<?php
/**
 * Post record as json
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * $_GET params
 * @param int "id_post"
 */

use hng2_modules\posts\posts_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

header("Content-Type: application/json; charset=utf-8");
if( $account->state != "enabled" ) die(json_encode(array("message" => trim($language->errors->access_denied) )));

$_GET["id_post"] = $_GET["id_post"] + 0;

if( empty($_GET["id_post"]) ) die(json_encode(array("message" => $current_module->language->messages->missing->id )));

$repository = new posts_repository();
$record = $repository->get($_GET["id_post"]);

if( is_null($record) ) die(json_encode(array("message" => $current_module->language->messages->post_not_found )));

if( $account->level < $config::MODERATOR_USER_LEVEL && $account->id_account != $record->id_author )
    die(json_encode(array("message" => $current_module->language->messages->post_not_yours )));

if( empty($record->id_featured_image) ) $record->featured_image_thumbnail = "";

$record->content = str_replace("<p> </p>",      "<p></p>", $record->content);
$record->content = str_replace("<p>&nbsp;</p>", "<p></p>", $record->content);

$data = $record->get_as_associative_array();
$config->globals["posts:json_record_exporter/working_item"] = $data;
$current_module->load_extensions("json_exporter", "before_output");
$data = $config->globals["posts:json_record_exporter/working_item"];
unset( $config->globals["posts:json_record_exporter/working_item"] );

$custom_fields = $record->fetch_all_metas(false);
if( ! empty($custom_fields) ) $data["custom_fields"] = $custom_fields;

echo json_encode(array("message" => "OK", "data" => $data));
