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

<link rel="stylesheet" type="text/css" href="<?= $config->full_root_path ?>/posts/media/styles~v<?=$config->scripts_version?>.css">

<script type="text/javascript"> var post_autosaver_heartbit = 30 * 1000; </script>
<script type="text/javascript" src="<?= $config->full_root_path ?>/posts/media/functions~v<?=$config->scripts_version?>.js"></script>

<script type="text/javascript">
    function reset_filter()
    {
        var $form = $('#filter_form');
        $form.find('input[name="search_for"]').val('');
        $form.find('select[name="limit"] option[value="20"]').prop('selected', true);
        $form.find('input[name="offset"]').val('0');
        $form.find('input[name="order"]').val('');
        
        $select = $form.find('select[name="search_category"]');
        if( $select.length > 0 ) $select.find('option:first').prop('selected', true);
        
        $select = $form.find('select[name="search_author"]');
        if( $select.length > 0 ) $select.find('option:first').prop('selected', true);
        
        $select = $form.find('select[name="search_pinned"]');
        if( $select.length > 0 ) $select.find('option:first').prop('selected', true);
        
        $form.find('select[name="search_status"] option:first').prop('selected', true);
    }
    
    //noinspection JSUnusedGlobalSymbols
    function paginate(value)
    {
        var $form = $('#filter_form');
        $form.find('input[name="offset"]').val(value);
        $form.submit();
    }

    function untrash_for_review(id_post)
    {
        var message = $('#untrash_for_review_confirmation').text();
        if( ! confirm(message) ) return;
    
        var url = $_FULL_ROOT_PATH + '/posts/scripts/toolbox.php';
        var params = {
            action:  'untrash_for_review',
            id_post: id_post,
            wasuuup: wasuuup()
        };
        
        $.blockUI(blockUI_default_params);
        $.get(url, params, function(response)
        {
            if( response != 'OK' )
            {
                alert(response);
                $.unblockUI();
                
                return;
            }
            
            location.href = $_FULL_ROOT_PATH + '/posts/?search_for=' + id_post + '&edit_post=' + id_post;
        });
    }
</script>

<div id="main_workarea">
    
    <?
    $url = "browser.php?wasuuup=" . md5(mt_rand(1, 65535));
    if( ! empty($_GET["edit_post"]) )  $url .= "&edit_post={$_GET["edit_post"]}";
    if( ! empty($_GET["create"]) )     $url .= "&create={$_GET["create"]}";
    if( ! empty($_GET["trash_post"]) ) $url .= "&trash_post={$_GET["trash_post"]}";
    if( ! empty($_GET["author"]) )     $url .= "&search_author={$_GET["author"]}";
    if( ! empty($_GET["search_for"]) ) $url .= "&search_for={$_GET["search_for"]}";
    ?>
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

<div id="untrash_for_review_confirmation" style="display: none;"><?= unindent($current_module->language->messages->untrash_for_review_confirmation) ?></div>
