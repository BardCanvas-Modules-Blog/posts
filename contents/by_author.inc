<?php
/**
 * Frontend index of posts by author
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var account $author
 */

use hng2_base\account;
use hng2_modules\posts\posts_repository;
use hng2_tools\record_browser;

$posts_repository      = new posts_repository();
$posts_browser         = new record_browser("");

$find_params = $posts_repository->build_find_params_for_author($author->id_account);
$posts_count = $posts_repository->get_record_count($find_params->where);
$posts       = $posts_repository->find($find_params->where, $find_params->limit, $find_params->offset, $find_params->order);
$pagination  = $posts_browser->build_pagination($posts_count, $find_params->limit, $find_params->offset);

$pagination_url_prefix = "$config->full_root_url/author/{$author->user_name}/posts/";
$template->set("pagination_url_prefix", $pagination_url_prefix);
?>

<h1 class="clearfix">
    <?= replace_escaped_vars($current_module->language->pages->by_author->title, '{$author}', $author->get_processed_display_name()) ?>
    <span class="pull-right"><?= $pagination["this_page_number"]; ?>/<?= $pagination["total_pages"]; ?></span>
</h1>

<? if($posts_count == 0)
{
    echo "
        <div class='framed_content state_ko'>
            <span class='fa fa-warning'>{$current_module->language->pages->by_author->no_records}</span>
        </div>
    ";
    
    return;
}
?>

<? include "{$template->abspath}/segments/post_index_renderer.inc"; ?>

