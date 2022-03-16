<?php
/**
 * Actual post saver - Called by save.php and toolbox.php
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var post_record $post
 * @var string      $date
 */

use hng2_base\config;
use hng2_modules\categories\categories_repository;
use hng2_modules\posts\post_record;
use hng2_modules\posts\toolbox;

$current_module->load_extensions("save_post", "initial_validations");

if( ! empty($post->id_post) )
{
    $time = (int) $settings->get("modules:posts.time_allowed_for_editing_after_publishing");
    if( ! $post->can_be_edited() )
    {
        if(empty($time)) die(unindent($current_module->language->messages->post_cannot_be_edited->without_timing));
        else             die(unindent($current_module->language->messages->post_cannot_be_edited->with_timing));
    }
}
else
{
    $config->globals["posts:submitted_post_is_new"] = true;
    $post->set_new_id();
    $post->id_author         = $account->id_account;
    $post->creation_date     = date("Y-m-d H:i:s");
    $post->creation_ip       = get_remote_address();
    $post->creation_host     = @gethostbyaddr($post->creation_ip);
    $post->creation_location = get_geoip_location_with_isp($post->creation_ip);
    $post->last_update       = date("Y-m-d H:i:s");
}

$excerpt_length = (int) $settings->get("modules:posts.excerpt_length");
if( empty($post->excerpt) ) $post->excerpt = make_excerpt_of(
    $post->content,
    empty($excerpt_length) ? 250 : $excerpt_length
);

if( empty($post->slug) )
{
    $post->slug = wp_sanitize_filename($post->title);
    if( empty($post->slug) || is_numeric($post->slug) ) $post->slug = "post-" . str_replace(".", "-", microtime(true));
    $existing_slugs = $repository->get_record_count(array(
        "id_post <>  '{$post->id_post}' and slug like '{$post->slug}%'"
    ));
    if( $existing_slugs > 0 ) $post->slug .= "-" . ($existing_slugs + 1);
}

$current_module->load_extensions("save_post", "after_record_forging");

# if( $post->main_category != $old_post->main_category )
#     $repository->unset_category($old_post->main_category, $post->id_post);
# $repository->set_category($_POST["main_category"], $post->id_post);

# Publishing date
if( $post->status == "published" && (empty($post->publishing_date) || $post->publishing_date == "0000-00-00 00:00:00") )
{
    if( empty($_POST["publishing_date"]) )
        $post->publishing_date = $date;
    else
        $post->publishing_date = $_POST["publishing_date"];
}

$enforced_expiration = $settings->get("modules:posts.enforced_expiration_by_category");
$set_expiration_date = "";
if(
    ! empty($enforced_expiration) 
    && $post->status == "published" && $post->publishing_date > "0000-00-00 00:00:00"
    && (empty($post->expiration_date) || $post->expiration_date == "0000-00-00 00:00:00")
) {
    $categories_repository = new categories_repository();
    $entries = explode("\n", $enforced_expiration);
    foreach($entries as $entry)
    {
        list($slug, $hours) = preg_split('/\s+-\s+/', $entry);
        $id = $categories_repository->get_id_by_slug($slug);
        if( $post->main_category != $id ) continue;
        
        $set_expiration_date = date("Y-m-d H:i:s", strtotime($post->publishing_date) + ($hours * 3600));
        break;
    }
}

# This goes here since tags are shared with media items
$tags = extract_hash_tags($post->title . " " . $post->content);
$featured_posts_tag = $settings->get("modules:posts.featured_posts_tag");
if(
    $account->level < config::MODERATOR_USER_LEVEL
    && $settings->get("modules:posts.show_featured_posts_tag_everywhere") != "true"
    && ! empty($featured_posts_tag)
    && in_array($featured_posts_tag, $tags)
) {
    unset($tags[array_search($featured_posts_tag, $tags)]);
    $post->title   = str_replace("#$featured_posts_tag", $featured_posts_tag, $post->title);
    $post->content = str_replace("#$featured_posts_tag", $featured_posts_tag, $post->content);
    $post->excerpt = str_replace("#$featured_posts_tag", $featured_posts_tag, $post->excerpt);
}

if( ! empty($_FILES["attachments"]) )
{
    # Coming from quick post... contents may be empty and only images/media are coming...
    $uploads = array();
    
    foreach($_FILES["attachments"] as $field => $types)
        foreach($types as $type => $entries)
            foreach($entries as $index => $value)
                $uploads[$type][$index][$field] = $value;
    
    foreach($uploads as $type => $files)
    {
        /** @var  array $file [name, type, tmp_name, error, size] */
        //foreach($files as $index => $file)
        //{
        //    $file_title = "{$account->display_name} - {$file["name"]}";
        //    
        //    if( $media_repository->get_record_count(array("title" => $file_title)) )
        //        die( $file["name"] . "\n" . $modules["gallery"]->language->messages->item_exists );
        //}
        
        /** @var  array $file [name, type, tmp_name, error, size] */
        foreach($files as $index => $file)
        {
            $file_title = "{$account->display_name} - {$file["name"]}";
            
            $item_data = array(
                "title"          => $file_title,
                "description"    => "{$post->title}\n\n{$post->excerpt}" .
                                    (empty($tags) ? "" : "\n\n#" . implode(" #", $tags)),
                "main_category"  => $post->main_category,
                "visibility"     => $post->visibility,
                "status"         => "published",
                "password"       => "",
                "allow_comments" => "1",
            );
            $res = $media_repository->receive_and_save($item_data, $file, true);
            
            if( is_string($res) ) die($res);
            
            if( $type == "image" )
                $post->content .= "\n<p><img src='{$res->get_item_url()}'
                    data-media-type='image' data-id-media='{$res->id_media}'></p>\n";
            else
                $post->content .= "\n<p><img src='{$res->get_thumbnail_url()}' 
                    data-media-type='video' data-id-media='{$res->id_media}'></p>\n";
            
            sleep(1);
        }
    }
}

$current_module->load_extensions("save_post", "before_extracting_media_items");

$media_items = array();
if( function_exists("extract_media_items") )
{
    $images = extract_media_items("image", $post->content);
    $videos = extract_media_items("video", $post->content);
    $media_items = array_merge($images, $videos);
    if( ! empty($post->featured_image_thumbnail) ) $media_items[] = $post->featured_image_thumbnail;
    $media_items = array_unique($media_items);
}

$current_module->load_extensions("save_post", "after_extracting_media_items");

if( $post->status == "published"
    && (empty($post->publishing_date) || $post->publishing_date == "0000-00-00 00:00:00") )
    $post->publishing_date = date("Y-m-d H:i:s");

$repository->save($post);
if( $set_expiration_date ) $repository->set_expiration_date($post->id_post, $set_expiration_date);

$config->globals["posts:saving_post_tags_list"] = $tags;
$current_module->load_extensions("save_post", "after_saving");
$tags = $config->globals["posts:saving_post_tags_list"];
unset( $config->globals["posts:saving_post_tags_list"] );

# Final override - pinned posts wont expire
if( $account->level >= config::MODERATOR_USER_LEVEL && ($post->pin_to_home || $post->pin_to_main_category_index) )
    $repository->set_expiration_date($post->id_post, "0000-00-00 00:00:00");

$media_deletions = array();
if( $post->status == "published" )
{
    $repository->set_tags($tags, $post->id_post);
    $media_deletions = $repository->set_media_items($media_items, $post->id_post);
}

$current_module->load_extensions("save_post", "before_eof");
$toolbox = new toolbox();
$toolbox->notify_mods_on_post_submission($post);

if( in_array($featured_posts_tag, $tags) )
    $mem_cache->delete("modules:posts.featured_posts");

if( ! empty($post->main_category_slug) )
    if( stristr($settings->get("modules:posts.slider_categories"), $post->main_category_slug) !== false )
        $mem_cache->delete("modules:posts.slider_posts");
