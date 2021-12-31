<?php
/**
 * Search extender: search by tag collection builder
 * Adds to the collection so it is properly displayed to the user
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Trailing globals:
 * @var module[] $modules
 * @var module   $current_module search
 * @var module   $this_module    self
 */

use hng2_base\module;
use hng2_modules\posts\posts_repository;
use hng2_modules\search\search_by_tag_item;

global $config, $template;

$tag        = addslashes($template->get("current_tag"));
$repository = new posts_repository();
$posts_data = $repository->get_for_tag($tag);

if( $posts_data->count == 0 ) return;

$collection = array();

foreach($posts_data->posts as $post)
    $collection[] = new search_by_tag_item(array(
        "type"      => "",
        "url"       => "{$config->full_root_url}/{$post->id_post}",
        "title"     => $post->get_processed_title(false),
        "excerpt"   => "[".time_today_string($post->publishing_date)."] " . $post->get_processed_excerpt(),
        "thumbnail" => $post->featured_image_thumbnail
    ));

$key = trim($this_module->language->extenders->search_by_tag_builder->collection_heading);
$config->globals["search_by_tag_collections"][$key] = $collection;
$config->globals["search_by_tag_counts"][$key]      = $posts_data->count;
