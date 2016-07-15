<?php
/**
 * Home index
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var posts_repository $posts_repository
 */

use hng2_modules\posts\posts_repository;
use hng2_tools\record_browser;

$posts_browser = new record_browser("");

$find_params = $posts_repository->build_find_params_for_home();
$posts_count = $posts_repository->get_record_count($find_params->where);
$posts       = $posts_repository->find($find_params->where, $find_params->limit, $find_params->offset, $find_params->order);
$pagination  = $posts_browser->build_pagination($posts_count, $find_params->limit, $find_params->offset);

$find_params    = $posts_repository->build_find_params_for_featured_posts();
$featured_posts = $posts_repository->find($find_params->where, $find_params->limit, $find_params->offset, $find_params->order);

include "{$template->abspath}/segments/post_index_home.inc";
