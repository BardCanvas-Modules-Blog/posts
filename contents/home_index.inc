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

$main_index_cache_for_guests_ttl = $settings->get("modules:posts.main_index_cache_for_guests");

# If caching is disabled...
if( empty($main_index_cache_for_guests_ttl) )
{
    $posts_data = $posts_repository->get_for_home(true);
    include "{$template->abspath}/segments/post_index_home.inc";
    
    return;
}

# If the visitor is not a guest
if( ! empty($main_index_cache_for_guests_ttl) && $account->level >= config::NEWCOMER_USER_LEVEL )
{
    $posts_data = $posts_repository->get_for_home(true);
    include "{$template->abspath}/segments/post_index_home.inc";
    
    return;
}

$offset = empty($_GET["offset"]) ? 0 : $_GET["offset"];
if( ! is_numeric($offset) ) $offset = 0;

# If the page is above 10
$items_per_page = $settings->get("modules:posts.items_per_page");
if( empty($items_per_page) ) $items_per_page = 30;
if( $offset >= $items_per_page * 10 )
{
    $posts_data = $posts_repository->get_for_home(true);
    include "{$template->abspath}/segments/post_index_home.inc";
    
    return;
}

#
# Cached code
#

$cache_version = $settings->get("modules:posts.main_index_cache_for_guests_version");
if( empty($cache_version) ) $cache_version = 1;
if($_GET["refresh_cache"] == "true") $cache_version++;

$cache_key      = "index_segments/home/posts_v{$cache_version}/offset:{$offset}";
$generating_key = "index_segments/home/posts/generating";

# Exists, let's push it
$res = $mem_cache->get($cache_key);
if( ! empty($res) )
{
    echo $res;
    return;
}

# Doesn't exist, let's build it
for($c = 1; $c <= 30; $c++ )
{
    if( $mem_cache->get($generating_key) ) break;
    
    sleep(1);
}

if( ! $mem_cache->get($generating_key) )
{
    # After waiting, the cache has been rebuilt. Let's output it.
    $res = $mem_cache->get($cache_key);
    if( ! empty($res) )
    {
        echo $res;
        return;
    }
}

# After waiting, the cache isn't rebuilt. Let's generate it.
$mem_cache->set($generating_key, true, 0, 30);

$posts_data = $posts_repository->get_for_home(true);
ob_start();
include "{$template->abspath}/segments/post_index_home.inc";
$contents = ob_get_flush();

$mem_cache->set($cache_key, $contents, 0, ($main_index_cache_for_guests_ttl * 60));
$mem_cache->delete($generating_key);

if($_GET["refresh_cache"] == "true") $settings->set("modules:posts.main_index_cache_for_guests_version", $cache_version);
