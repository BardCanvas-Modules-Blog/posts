<?php
namespace hng2_modules\posts;

use hng2_base\config;
use hng2_base\module;
use hng2_modules\categories\categories_repository;

class toolbox
{
    /**
     * @var categories_repository
     */
    private $categories_repository;
    
    private $categories_by_slug;
    
    public function __construct()
    {
        $this->categories_repository = new categories_repository();
    }
    
    /**
     * Returns a list of messages per category id
     * 
     * @return array [id => message, ...]
     */
    public function get_per_category_messages()
    {
        global $settings;
        
        if( empty($this->categories_by_slug) )
            $this->categories_by_slug = $this->categories_repository->get_ids_by_slug();
        
        $messages = $settings->get("modules:posts.per_category_messages");
        if( empty($messages) ) return array();
        
        $list_by_id = array();
        
        $lines = explode("\n", trim($messages));
        foreach($lines as $line)
        {
            $line = trim($line);
            if( substr($line, 0, 1) == "#" ) continue;
            
            list($slug, $text) = explode("|", $line);
            $slug = trim($slug);
            $text = trim($text);
            
            if( empty($this->categories_by_slug[$slug]) ) continue;
            
            $id = $this->categories_by_slug[$slug];
            
            $list_by_id[$id][] = $text;
        }
        
        $return = array();
        foreach($list_by_id as $id => $lines) $return[$id] = implode("<br>\n", $lines);
        
        return $return;
    }
    
    /**
     * Notify mods when a post is published
     * 
     * @param post_record $post
     */
    public function notify_mods_on_post_submission(post_record $post)
    {
        /**
         * @var posts_repository $repository
         * @var module           $current_module posts
         */
        global $config, $settings, $repository, $account, $current_module;
        
        $mem_ttl = 60*60;
        
        if( $account->level >= config::MODERATOR_USER_LEVEL ) return;
        if( $post->status == "draft" ) return;
        if( $post->creation_date > date("Y-m-d H:i:s", strtotime("$post->creation_date + $mem_ttl seconds")) ) return;
        
        $post_author = $post->get_author();
        $category    = $this->categories_repository->get($post->main_category);
        
        $subject = replace_escaped_vars(
            $current_module->language->email_templates->post_submitted->subject,
            array(
                '{$website_name}',
                '{$post_author}',
                '{$post_title}',
            ),
            array(
                $settings->get("engine.website_name"),
                $post_author->display_name,
                $post->title,
            )
        );
        
        $user_ip  = get_user_ip(); $parts = explode(".", $user_ip); array_pop($parts);
        $segment  = implode(".", $parts);
        $boundary = date("Y-m-d H:i:s", strtotime("now - 7 days"));
        $where = array(
            "status = 'published'",
            "visibility = 'public'",
            "publishing_date >= '$boundary'",
            "creation_ip like '{$segment}.%'",
            "id_post <> '$post->id_post'",
        );
        $other_posts_from_segment = $repository->find($where, 12, 0, "publishing_date desc");
        if( count($other_posts_from_segment) == 0 )
        {
            $other_posts_from_segment = "<li>{$current_module->language->email_templates->post_submitted->none_found}</li>";
        }
        else
        {
            $lis = "";
            foreach($other_posts_from_segment as $other_post)
            {
                $published   = time_mini_string($other_post->publishing_date);
                $link        = $other_post->get_permalink(true);
                $author_link = "{$config->full_root_url}/user/{$other_post->author_user_name}";
                $lis        .= "<li><a href='$author_link'>{$other_post->author_display_name}</a>
                                    [$published • {$other_post->creation_ip}]:
                                    <a href='{$link}'>{$other_post->title}</a></li>";
            }
            $other_posts_from_segment = $lis;
        }
        
        $body = replace_escaped_vars(
            $current_module->language->email_templates->post_submitted->body,
            array(
                '{$post_author}',
                '{$main_category}',
                '{$post_link}',
                '{$post_title}',
                '{$excerpt}',
                '{$featured_image}',
                '{$ip}',
                '{$location}',
                '{$user_agent}',
                '{$other_posts_from_segment}',
                '{$post_url}',
                '{$edit_url}',
                '{$preferences}',
                '{$website_name}',
            ),
            array(
                "<a href='{$config->full_root_url}/user/{$post_author->user_name}'>$post_author->display_name</a>",
                "<a href='{$config->full_root_url}/category/{$category->slug}'>{$category->title}</a>",
                "{$config->full_root_url}/{$post->id_post}",
                $post->title,
                empty($post->excerpt) ? "&mdash;" : $post->excerpt,
                empty($post->featured_image_thumbnail)
                    ? "<p>{$current_module->language->email_templates->post_submitted->none_defined}</p>"
                    : "<img height='200' border='1' src='{$config->full_root_url}{$post->featured_image_thumbnail}'>",
                $user_ip,
                get_geoip_location_with_isp($user_ip),
                $_SERVER["HTTP_USER_AGENT"],
                $other_posts_from_segment,
                "{$config->full_root_url}/{$post->id_post}",
                "{$config->full_root_url}/posts/?edit_post={$post->id_post}&wasuuup=" . md5(mt_rand(1, 65535)),
                "{$config->full_root_url}/accounts/preferences.php",
                $settings->get("engine.website_name"),
            )
        );
        
        $body = unindent($body);
        broadcast_mail_to_moderators(
            $subject, $body, "@posts:moderator_emails_for_posts", array($post->id_author)
        );
    }
    
    /**
     * @param post_record[] $tree
     * @param string        $selected_post_id
     *
     * @return string
     */
    public function build_post_family_tree_links($tree, $selected_post_id = "")
    {
        $return = "<ul>\n";
        foreach($tree as $id_post => $post)
        {
            $class   = $post->id_post == $selected_post_id ? "selected" : "";
            $return .= "<li>\n";
            $return .= "<a class='{$class}' href='{$post->get_permalink()}'>{$post->title}</a>\n";
            if( ! empty($post->children) ) $return .= $this->build_post_family_tree_links($post->children, $selected_post_id);
            $return .= "</li>\n";
        }
        $return .= "</ul>\n";
        
        return $return;
    }
}
