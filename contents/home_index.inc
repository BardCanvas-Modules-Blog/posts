<?php
/**
 * Home index
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var template         $template
 * @var posts_repository $posts_repository
 */

use hng2_base\template;
use hng2_modules\posts\posts_repository;

$posts_data = $posts_repository->get_for_home();
include "{$template->abspath}/segments/post_index_home.inc";
