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

if( empty($main_index_cache_for_guests_ttl) )
{
    # Caching is disabled
    $posts_data = $posts_repository->get_for_home(true);
    include "{$template->abspath}/segments/post_index_home.inc";
    
    return;
}

if( ! empty($main_index_cache_for_guests_ttl) && $account->level >= config::NEWCOMER_USER_LEVEL )
{
    # We have a registered user account in
    $posts_data = $posts_repository->get_for_home(true);
    include "{$template->abspath}/segments/post_index_home.inc";
    
    return;
}

#
# Cached code
#

$cache_version = $settings->get("modules:posts.main_index_cache_for_guests_version");
if( empty($cache_version) ) $cache_version = 1;

$offset = empty($_GET["offset"]) ? 0 : $_GET["offset"];
if( ! is_numeric($offset) ) $offset = 0;
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

for($c = 1; $c <= 15; $c++ )
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

$mem_cache->set($generating_key, true, 0, 10);

$posts_data = $posts_repository->get_for_home(true);

ob_start();
include "{$template->abspath}/segments/post_index_home.inc";
$contents = ob_get_flush();

$cache_version++;
$cache_key = "index_segments/home/posts_v{$cache_version}/offset:{$offset}";

$mem_cache->set($cache_key, $contents, 0, ($main_index_cache_for_guests_ttl * 60));
$mem_cache->delete($generating_key);
$settings->set("modules:posts.main_index_cache_for_guests_version", $cache_version);
