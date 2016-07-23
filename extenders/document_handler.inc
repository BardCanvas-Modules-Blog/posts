<?php
/**
 * Document handler for posts - By id, slug, etc.
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * $_GET params:
 * @param handle
 */

use hng2_modules\posts\posts_repository;

if( empty($_GET["handle"]) ) return;

$handle = trim(stripslashes($_GET["handle"]));

$repository = new posts_repository();
$post = $repository->get_by_id_or_slug($handle);

if( is_null($post) ) return;

$template->page_contents_include = "single.inc";
$template->set_page_title($post->title);
include "{$template->abspath}/main.php";
exit;
