<?php
/**
 * Media record class extender: tags list filtering
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

global $config;
global $settings;

$list = $config->globals["media_record/filtering_tags_list"];
if( empty($list) ) return;

if( is_string($list) ) $list = explode(",", $list);

$featureds_tag = $settings->get("modules:posts.featured_posts_tag");
if( empty($featureds_tag) ) return;
if( $settings->get("modules:posts.show_featured_posts_tag_everywhere") == "true" ) return;

$key = array_search($featureds_tag, $list);
if( $key === false ) return;

unset($list[$key]);
$config->globals["media_record/filtering_tags_list"] = $list;
