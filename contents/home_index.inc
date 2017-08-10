<?php
/**
 * Home index
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var template         $template
 * @var posts_repository $posts_repository
 */

use hng2_base\config;
use hng2_base\template;
use hng2_modules\posts\posts_repository;

if( $settings->get("modules:posts.disable_posts_index_on_home") == "true" ) return;

$offset = empty($_GET["offset"]) ? 0 : $_GET["offset"];
if( ! is_numeric($offset) ) $offset = 0;

# If the page is above 10
$items_per_page = $settings->get("modules:posts.items_per_page");
if( empty($items_per_page) ) $items_per_page = 30;
if( $offset >= $items_per_page * 10 )
{
    $posts_data = $posts_repository->get_for_home(true);
    echo "\n\n<!-- @caching: Pages above 10 aren't cached -->\n\n";
    include "{$template->abspath}/segments/post_index_home.inc";
    
    return;
}

#
# Cached code
#

if( $account->level < config::NEWCOMER_USER_LEVEL )
{
    $cache_ttl = $settings->get("modules:posts.main_index_cache_for_guests");
    if( empty($cache_ttl) )
    {
        $posts_data = $posts_repository->get_for_home(true);
        echo "\n\n<!-- @caching: Caching for guests disabled, real time contents ahead -->\n\n";
        include "{$template->abspath}/segments/post_index_home.inc";
        
        return;
    }
    
    $cache_version = $settings->get("modules:posts.main_index_cache_for_guests_version");
    if( empty($cache_version) ) $cache_version = 1;
    $cache_key = "index_segments/home/posts_v{$cache_version}/offset:{$offset}";
    $cache_ttl = $cache_ttl * 60;
}
else
{
    $cache_ttl = (int) $settings->get("modules:posts.main_index_cache_for_users");
    if( empty($cache_ttl) )
    {
        $posts_data = $posts_repository->get_for_home(true);
        echo "\n\n<!-- @caching: Caching for online users disabled -->\n\n";
        include "{$template->abspath}/segments/post_index_home.inc";
        
        return;
    }
    
    $cache_version = $settings->get("modules:posts.main_index_cache_for_users_version");
    if( empty($cache_version) ) $cache_version = 1;
    $cache_key = "index_segments/home/posts_v{$cache_version}/level:{$account->level};offset:{$offset}";
    $cache_ttl = $cache_ttl * 60;
}

$contents  = $mem_cache->get($cache_key);
if( ! empty($contents) )
{
    echo "\n\n<!-- @caching: Cached contents ahead for level {$account->level} -->\n\n";
    echo $contents;
    
    return;
}

$posts_data = $posts_repository->get_for_home(true);

echo "\n\n<!-- @caching: Contents to be cached ahead -->\n\n";
ob_start();
include "{$template->abspath}/segments/post_index_home.inc";
$contents = ob_get_flush();
$mem_cache->set($cache_key, $contents, 0, $cache_ttl);
