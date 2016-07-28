<?php
/**
 * Frontend index of posts by author
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var config $config
 * @var template $template
 * @var account $author
 */

use hng2_base\account;
use hng2_base\config;
use hng2_base\template;
use hng2_modules\posts\posts_repository;

$posts_repository      = new posts_repository();
$posts_data            = $posts_repository->get_for_author($author->id_account);
$pagination_url_prefix = "$config->full_root_url/author/{$author->user_name}/posts/";
$template->set("pagination_url_prefix", $pagination_url_prefix);
?>

<h1 class="clearfix">
    <?= replace_escaped_vars($current_module->language->pages->by_author->title, '{$author}', $author->get_processed_display_name()) ?>
    <span class="pull-right"><?= $posts_data->pagination["this_page_number"]; ?>/<?= $posts_data->pagination["total_pages"]; ?></span>
</h1>

<? if($posts_data->count == 0): ?>
    <div class='framed_content state_ko'>
        <span class='fa fa-warning'><?= $current_module->language->pages->by_author->no_records ?></span>
    </div>
    <? return; ?>
<? endif; ?>

<? include "{$template->abspath}/segments/post_index_renderer.inc"; ?>
