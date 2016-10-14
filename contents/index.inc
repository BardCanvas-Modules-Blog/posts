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
        
        $form.find('input[name="search_status"][value=""]').prop('checked', true);
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
    
    function empty_trash()
    {
        var message = $('#empty_trash_confirmation').text();
        if( ! confirm(message) ) return;
        
        var url = $_FULL_ROOT_PATH + '/posts/scripts/toolbox.php?action=empty_trash&wasuuup=' + wasuuup();
        $.blockUI(blockUI_default_params);
        $.get(url, function(response)
        {
            if( response != 'OK' )
            {
                alert(response);
                $.unblockUI();
                return;
            }
            
            $.unblockUI();
            $('#refresh_posts_browser').click();
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

<div id="untrash_for_review_confirmation"   style="display: none;"><?= unindent($current_module->language->messages->untrash_for_review_confirmation) ?></div>

<!-- Multi-record actions -->
<script type="text/javascript">
    
    function toggle_record_selections(trigger)
    {
        var $trigger = $(trigger);
        var $targets = $trigger.closest('table').find('input.record_selector');
        
        if( $trigger.is(':checked') ) $targets.prop('checked', true);
        else                          $targets.prop('checked', false);
    }
    
    var total_multirecord_action_threads     = 0;
    var processed_multirecord_action_threads = 0;
    var multirecord_action_threads_interval  = null;
    
    function launch_multirecord_actions(trigger, action)
    {
        var $trigger = $(trigger);
        var $targets = $trigger.closest('.table_wrapper').find('input.record_selector:checked');
        
        var targets_count = $targets.length;
        if( targets_count == 0 )
        {
            var message = $('#multirecord_actions_none_selected').text();
            alert(message);
            return;
        }
        
        var action_caption = $trigger.text().trim();
        
        var confirmation = $('#multirecord_actions_confirmation').text();
        confirmation = confirmation.replace(/\{\$amount\}/g,   targets_count);
        confirmation = confirmation.replace(/\{\$new_flag\}/g, action_caption);
        
        if( ! confirm(confirmation) ) return;
        
        total_multirecord_action_threads     = targets_count;
        processed_multirecord_action_threads = 0;
        $trigger.closest('.multi_record_actions').block(blockUI_smallest_params);
        
        multirecord_action_threads_interval = setInterval('check_multirecord_threads()', 500);
        
        $targets.each(function()
        {
            var $this      = $(this);
            var $container = $this.closest('tr');
            var id         = $this.val();
            
            launch_multirecord_action(action, id, $container);
        });
    }
    
    function check_multirecord_threads()
    {
        console.log( 'threads: ', processed_multirecord_action_threads, ' / ',  total_multirecord_action_threads);
        
        if( processed_multirecord_action_threads < total_multirecord_action_threads ) return;
        
        clearInterval(multirecord_action_threads_interval);
        
        var message = $('#multirecord_actions_all_done').text();
        alert(message);
        
        $('.multirecord_actions_table_refresher').click();
    }
    
    function launch_multirecord_action(action, id, $container)
    {
        var url    = $_FULL_ROOT_PATH + '/posts/scripts/toolbox.php';
        var params = {
            action:     'change_status',
            id_post:    id,
            new_status: action,
            wasuuup:    wasuuup()
        };
        
        $container.block(blockUI_smaller_params);
        $.get(url, params, function(response)
        {
            if( response != 'OK' )
            {
                throw_notification(response, 'error');
                $container.unblock();
                processed_multirecord_action_threads++;
                
                return;
            }
            
            $container.unblock().fadeOut('fast', function()
            {
                $(this).remove();
                processed_multirecord_action_threads++;
            });
        });
    }
    
</script>
<div id="multirecord_actions_confirmation"  style="display: none;"><?= unindent($current_module->language->multi_actions->confirmation) ?></div>
<div id="multirecord_actions_none_selected" style="display: none;"><?= unindent($current_module->language->multi_actions->none_selected) ?></div>
<div id="multirecord_actions_all_done"      style="display: none;"><?= unindent($current_module->language->multi_actions->done) ?></div>
<div id="empty_trash_confirmation"          style="display: none;"><?= unindent($current_module->language->empty_trash->confirmation) ?></div>
