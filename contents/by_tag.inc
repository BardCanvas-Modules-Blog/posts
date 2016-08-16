<?php
/**
 * Frontend index of posts by hashtag
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var config   $config
 * @var template $template
 * @var string   $tag
 */

use hng2_base\config;
use hng2_base\template;
use hng2_modules\posts\posts_repository;

$posts_repository      = new posts_repository();
$posts_data            = $posts_repository->get_for_tag($tag);
$pagination_url_prefix = "$config->full_root_url/tag/{$tag}/";
$template->set("pagination_url_prefix", $pagination_url_prefix);
?>

<h1 class="clearfix">
    <?= replace_escaped_vars($current_module->language->pages->by_tag->title, '{$tag}', $tag) ?>
    <span class="pull-right"><?= $posts_data->pagination["this_page_number"]; ?>/<?= $posts_data->pagination["total_pages"]; ?></span>
</h1>

<? if($posts_data->count == 0): ?>
    <div class='framed_content state_ko'>
        <span class='fa fa-warning'><?= replace_escaped_vars(
                $current_module->language->pages->by_tag->no_records,
                '{$link}',
                "{$config->full_root_path}/tag/{$tag}/media"
            ) ?></span>
    </div>
<? endif; ?>

<? include "{$template->abspath}/segments/post_index_renderer.inc"; ?>