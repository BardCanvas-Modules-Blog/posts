<?php
/**
 * Frontend index of posts by category
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var template $template
 * 
 * $_GET params:
 * @param slug
 */

include "../config.php";
include "../includes/bootstrap.inc";

use hng2_base\template;
use hng2_modules\categories\categories_repository;

try { check_sql_injection($_GET); }
catch(\Exception $e) { throw_fake_501(); }

$categories_repository = new categories_repository();

if( empty($_GET["slug"]) ) throw_fake_404();

$category = $categories_repository->get($_GET["slug"]);
if( is_null($category) ) throw_fake_404();

$template->set("page_tag",               "post_category_index");
$template->set("showing_archive",        true);
$template->set("current_category_slug",  $category->slug);
$template->set("current_category_title", $category->title);
$template->set("current_category_id",    $category->id_category);
$template->append("additional_body_attributes", " data-category-slug='$category->slug' data-category-id='$category->id_category'");

$template->page_contents_include = "by_category.inc";
$template->set_page_title(replace_escaped_vars(
    $current_module->language->pages->by_category->title, '{$category}', $category->title
));
$template->append("additional_body_attributes", " data-listing-type='archive'");
include "{$template->abspath}/main.php";
