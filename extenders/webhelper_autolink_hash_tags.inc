<?php
/**
 * Web Helper extender: tags list filtering on hashtags autolinking
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

global $config;
global $settings;

$list = $config->globals["webhelper/filtering_tags_list"];

if( empty($list) ) return;

$featureds_tag = $settings->get("modules:posts.featured_posts_tag");
if( empty($featureds_tag) ) return;
if( $settings->get("modules:posts.show_featured_posts_tag_everywhere") == "true" ) return;

if( isset($list["#{$featureds_tag}"]) ) $list["#{$featureds_tag}"] = "";

$config->globals["webhelper/filtering_tags_list"] = $list;
