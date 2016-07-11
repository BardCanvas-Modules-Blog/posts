<?php
/**
 * Post saver
 *
 * @package    BardCanvas
 * @subpackage categories
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

use hng2_modules\posts\posts_repository;

include "../../config.php";
include "../../includes/bootstrap.inc";
if( ! $account->_is_admin ) throw_fake_404();

if( empty($_POST["title"]) )    die($current_module->language->messages->missing->title);
if( empty($_POST["slug"]) )     die($current_module->language->messages->missing->slug);
if( empty($_POST["content"]) )  die($current_module->language->messages->missing->content);






$repository = new posts_repository();



print_r($_POST);
