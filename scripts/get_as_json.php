<?php
/**
 * Post record as json
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * $_GET params
 * @param string "id_post"
 */

use hng2_modules\posts\posts_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

header("Content-Type: application/json; charset=utf-8");
if( ! $account->_exists ) die(json_encode(array("message" => $language->errors->page_requires_login )));

if( empty($_GET["id_post"]) ) die(json_encode(array("message" => $current_module->language->messages->missing->id )));

$repository = new posts_repository();
$record = $repository->get($_GET["id_post"]);

if( is_null($record) ) die(json_encode(array("message" => $current_module->language->messages->post_not_found )));

if( empty($record->id_featured_image) ) $record->featured_image_thumbnail = "";

echo json_encode(array("message" => "OK", "data" => $record->get_as_associative_array()));
