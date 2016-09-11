<?php
/**
 * Contact extender: sets subject and body when the contact form is invoked
 * via GET with the next arguments: action=report & type=comment & id=xxxxxxx
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * Trailing vars:
 * @var module     $this_module self (commments)
 *
 * Imported config globals:
 * • string @contact:subject
 * • string @contact:content
 * • array  @contact:show_errors
 */

use hng2_base\module;
use hng2_modules\posts\posts_repository;

global $config;

if( empty($_GET["action"]) ) return;
if( empty($_GET["type"]) ) return;
if( $_GET["type"] != "post" ) return;

if( empty($_GET["id"]) )
{
    $config->globals["@contact:show_errors"][] = $this_module->language->reporting->id_not_set;
    
    return;
}

$repository = new posts_repository();
$post       = $repository->get($_GET["id"]);

if( is_null($post) )
{
    $config->globals["@contact:show_errors"][] = $this_module->language->reporting->not_found;
    
    return;
}

if( in_array($post->status, array("rejected", "spam", "trashed")) )
{
    $config->globals["@contact:show_errors"][] = $this_module->language->reporting->already_processed;
    
    return;
}

if( in_array($post->status, array("reviewing")) )
{
    $config->globals["@contact:show_errors"][] = $this_module->language->reporting->in_review;
    
    return;
}

$config->globals["@contact:subject"] = replace_escaped_vars(
    $this_module->language->reporting->subject,
    array('{$post_id}', '{$post_title}', '{$post_author}'),
    array($post->id_post, $post->title, $post->author_display_name)
);

$config->globals["@contact:content"] = replace_escaped_vars(
    $this_module->language->reporting->content,
    array('{$post_author}', '{$date}', '{$excerpt}', '{$permalink}'),
    array(
        $post->author_display_name,
        $post->creation_date . " (" . time_elapsed_string($post->creation_date). ")",
        $post->excerpt,
        $post->get_permalink(true)
    )
);
