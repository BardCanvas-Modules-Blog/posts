<?php
/**
 * Save post publishing date
 *
 * @package    BardCanvas
 * @subpackage categories
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * $_POST params
 * @param int    id_post
 * @param string date
 */

use hng2_base\config;
use hng2_modules\posts\posts_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

header("Content-Type: text/plain; charset=utf-8");
if( $account->state != "enabled" ) die($language->errors->access_denied);

$_POST["id_post"] = $_POST["id_post"] + 0;

if( empty($_POST["id_post"]) ) die($current_module->language->messages->missing->id);

$parts = explode(" ", $_POST["date"]);
if( count($parts) != 2 ) die($current_module->language->messages->invalid_publishing_date);

$date_parts = explode("-", $parts[0]);
if( count($date_parts) != 3 ) die($current_module->language->messages->invalid_publishing_date);

$time_parts = explode(":", $parts[1]);
if( count($time_parts) < 2 ) die($current_module->language->messages->invalid_publishing_date);
if( $time_parts[0] < 0 || $time_parts[0] > 23 ) die($current_module->language->messages->invalid_publishing_date);
if( $time_parts[1] < 0 || $time_parts[1] > 59 ) die($current_module->language->messages->invalid_publishing_date);

if( ! checkdate($date_parts[1], $date_parts[2], $date_parts[0]) )
    die($current_module->language->messages->invalid_publishing_date);

if( $_POST["date"] < date("Y-m-d H:i:s") )
    die($current_module->language->messages->publishing_date_in_the_past);

$repository = new posts_repository();
$post = $repository->get($_POST["id_post"]);
if( is_null($post) ) die($current_module->language->messages->post_not_found);

if( $account->level < config::MODERATOR_USER_LEVEL && $account->id_account != $post->id_author )
    die($current_module->language->messages->cannot_change_others_posts);

if( $post->expiration_date != "" && $post->expiration_date != "0000-00-00 00:00:00" )
    die($current_module->language->messages->cannot_schedule_posts_with_expiration);

$old_post = clone $post;
$post->publishing_date = $_POST["date"];
$repository->save($post);
$current_module->load_extensions("save_post", "before_eof");

echo "OK";
