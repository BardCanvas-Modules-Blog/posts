<?php
/**
 * Post trashes
 *
 * @package    BardCanvas
 * @subpackage categories
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * $_GET params
 * @param id_post
 */

use hng2_modules\posts\posts_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";

header("Content-Type: text/plain; charset=utf-8");
if( ! $account->_exists ) die($language->errors->page_requires_login);

if( empty($_GET["id_post"]) ) die($current_module->language->messages->missing->id);

$repository = new posts_repository();
$count      = $repository->get_record_count(array("id_post" => $_GET["id_post"]));

if( $count == 0 ) die($current_module->language->messages->post_not_found);

$repository->trash($_GET["id_post"]);

echo "OK";
