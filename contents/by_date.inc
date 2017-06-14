<?php
/**
 * Frontend index of posts by date
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var config $config
 * @var template $template
 * @var string $showing_date
 * @var string $start_date
 * @var string $end_date
 */

use hng2_base\config;
use hng2_base\template;
use hng2_modules\posts\posts_repository;
use hng2_tools\record_browser;

$current_module->load_extensions("posts_by_date_index", "pre_fetching");

$posts_repository      = new posts_repository();
$posts_browser         = new record_browser("");

$posts_repository      = new posts_repository();
$posts_data            = $posts_repository->get_for_date_range($start_date, $end_date);
$pagination_url_prefix = "$config->full_root_url/date/{$_GET["date"]}/";
$template->set("pagination_url_prefix", $pagination_url_prefix);

$current_module->load_extensions("posts_by_date_index", "pre_rendering");
?>

<? $current_module->load_extensions("posts_by_date_index", "before_heading"); ?>

<h1 class="clearfix">
    <?= replace_escaped_vars($current_module->language->pages->by_date->title, '{$date}', $showing_date) ?>
    <a class="small" href="<?= $config->full_root_url ?>/date/<?= $_GET["date"] ?>/media/"><?= $current_module->language->pages->by_date->see_media ?></a>
    <span class="pull-right"><?= $posts_data->pagination["this_page_number"]; ?>/<?= $posts_data->pagination["total_pages"]; ?></span>
</h1>

<? $current_module->load_extensions("posts_by_date_index", "after_heading"); ?>

<? if($posts_data->count == 0): ?>
    <div class='framed_content state_ko'>
        <span class='fa fa-warning'><?= $current_module->language->pages->by_date->no_records ?></span>
    </div>
    <? return; ?>
<? endif; ?>

<? include "{$template->abspath}/segments/post_index_renderer.inc"; ?>
