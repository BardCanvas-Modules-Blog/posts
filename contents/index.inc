<?php
/**
 * Posts index
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */
?>

<script type="text/javascript" src="<?= $config->full_root_path ?>/lib/jquery.exptextarea.js"></script>
<script type="text/javascript">$(document).ready(function() { $('.expandible_textarea').expandingTextArea() });</script>

<script type="text/javascript" src="<?= $config->full_root_path ?>/lib/tinymce-4.4.0/tinymce.min.js"></script>
<script type="text/javascript" src="<?= $config->full_root_path ?>/posts/media/init_post_composer~v<?=$config->scripts_version?>.js"></script>
<script type="text/javascript"> tinymce.init(post_composer_tinymce_defaults) </script>

<link rel="stylesheet" type="text/css" href="<?= $config->full_root_path ?>/posts/media/styles~v<?=$config->scripts_version?>.css">
<script type="text/javascript"          src="<?= $config->full_root_path ?>/posts/media/functions~v<?=$config->scripts_version?>.js"></script>

<div id="main_workarea">
    
    <? $url = "browser.php?wasuuup=" . md5(mt_rand(1, 65535)); ?>
    <div id="posts_browser" class="ajax_record_browser" data-src="<?= $url ?>">
        <div class="framed_content state_highlight" style="padding: 100px 0; text-align: center;">
            <span class="fa fa-spinner fa-pulse"></span>
            <?= $language->wait ?>
        </div>
    </div>
    
</div>

<div id="form_workarea" style="display: none;">
    
    <h1 class="clearfix for_addition">
        <button class="pull-right" onclick="hide_post_form()">
            <span class="fa fa-reply"></span>
            <?= $language->return ?>
        </button>
        
        <?= $current_module->language->form->add_title; ?>
    </h1>
    
    <h1 class="clearfix for_edition" style="display: none">
        <button class="pull-right" onclick="hide_post_form()">
            <span class="fa fa-reply"></span>
            <?= $language->return ?>
        </button>
        
        <?= $current_module->language->form->edit_title; ?>
    </h1>
    
    <? include __DIR__ . "/form.inc"; ?>
</div>
