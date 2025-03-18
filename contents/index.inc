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
<style type="text/css">
    #post_form .field[data-field="publishing_date"] .fieldset .ui-datepicker-trigger {
        cursor: pointer; vertical-align: middle;
    }
</style>

<script type="text/javascript"> var post_permalink_style = '<?= $settings->get("modules:posts.permalink_style") ?>'; </script>
<script type="text/javascript"> var post_autosaver_heartbit = 30 * 1000; </script>
<script type="text/javascript"> var posts_category_selector_post_process_extensions = {}; </script>
<script type="text/javascript"> var post_form_iframed_mode = '<?= $_GET["post_form_iframed_mode"] ?>' == 'true'; </script>
<script type="text/javascript" src="<?= $config->full_root_path ?>/posts/media/functions~v1.22.0-3.js"></script>
<script type="text/javascript" src="<?= $config->full_root_path ?>/posts/media/moment.js"></script>

<? $current_module->load_extensions("browser", "index_extra_inits"); ?>

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

<div id="main_workarea" style="<? if($_GET["hide_browser"] == "true") echo "display: none;" ?>">
    <?
    $url = "browser.php?wasuuup=" . md5(mt_rand(1, 65535));
    if( ! empty($_GET["edit_post"]) )  $url .= "&edit_post={$_GET["edit_post"]}";
    if( ! empty($_GET["create"]) )     $url .= "&create={$_GET["create"]}";
    if( ! empty($_GET["trash_post"]) ) $url .= "&trash_post={$_GET["trash_post"]}";
    if( ! empty($_GET["author"]) )     $url .= "&search_author={$_GET["author"]}";
    if( ! empty($_GET["search_for"]) ) $url .= "&search_for={$_GET["search_for"]}";
    
    # BCM additions
    if( ! empty($_GET["bcm_platform"]) )      $url .= "&bcm_platform={$_GET["bcm_platform"]}";
    if( ! empty($_GET["bcm_version"]) )       $url .= "&bcm_version={$_GET["bcm_version"]}";
    if( ! empty($_GET["bcm_access_token"]) )  $url .= "&bcm_access_token={$_GET["bcm_access_token"]}";
    if( ! empty($_GET["bcm_force_session"]) ) $url .= "&bcm_force_session={$_GET["bcm_force_session"]}";
    if( ! empty($_GET["bcm_output_type"]) )   $url .= "&bcm_output_type={$_GET["bcm_output_type"]}";
    if( ! empty($_GET["hide_browser"]) )      $url .= "&hide_browser={$_GET["hide_browser"]}";
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
        <? if( $_GET["hide_browser"] != "true" ): ?>
            <button class="pull-right" onclick="hide_post_form()">
                <span class="fa fa-reply"></span>
                <?= $language->return ?>
            </button>
        <? endif; ?>
        
        <?= $current_module->language->form->edit_title; ?>
    </h1>
    
    <? include __DIR__ . "/form.inc"; ?>
</div>

<div id="iframed_close_message" style="display: none;">
    
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
        confirmation = confirmation.replace(/{\$amount}/g,   targets_count);
        confirmation = confirmation.replace(/{\$new_flag}/g, action_caption);
        
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

<div id="schedule_editor" style="display: none; padding: 10px;" class="aligncenter"
    data-ok-caption="<?= $language->ok ?>" data-cancel-caption="<?= $language->cancel ?>"
    title="<?= $current_module->language->form->fields->publishing_date->changer_dialog_caption ?>">
    
    <input type="hidden" id="schedule_id_post">
    
    <div id="schedule_date_control"></div>
    
    <br>
    <i class="fa fa-clock-o"></i>
    <input type="text" id="schedule_time_control" value="" maxlength="5"
           style="width: 60px; text-align: center;">
</div>
<script type="text/javascript">
    
    function show_schedule_changer(trigger, pubdate, id_post)
    {
        var $trigger = $(trigger);
        var parts    = pubdate.split(' ');
        var _date    = parts[0];
        var _time    = parts[1].substring(0, 5);
        
        var $dialog      = $('#schedule_editor');
        var $datecontrol = $('#schedule_date_control');
        var $timecontrol = $('#schedule_time_control');
        var $idpostfield = $('#schedule_id_post');
        
        $dialog.data('current_date', pubdate.substring(0, 16));
        $idpostfield.val(id_post);
        $timecontrol.val(_time);
        $datecontrol.datepicker('setDate', _date);
        $dialog.dialog('open');
    }
    
    function save_post_publishing_date()
    {
        var $dialog = $('#schedule_editor');
        var _date   = moment($('#schedule_date_control').datepicker('getDate')).format('YYYY-MM-DD');
        var _time   = $('#schedule_time_control').val();
        
        console.log( $dialog.data('current_date') );
        if( $dialog.data('current_date') == (_date + ' ' + _time) )
        {
            $dialog.dialog('close');
            
            return;
        }
        
        var id      = $('#schedule_id_post').val();
        var url     = $_FULL_ROOT_PATH + '/posts/scripts/set_publishing_date.php';
        var params  = {
            id_post: id,
            date:    _date + ' ' + _time + ':00',
            wasuuup: wasuuup()
        };
        
        $dialog.closest('.ui-dialog').block(blockUI_medium_params);
        $.post(url, params, function(response)
        {
            if( response != 'OK' )
            {
                alert(response);
    
                $dialog.closest('.ui-dialog').unblock();
                return;
            }
            
            $dialog.closest('.ui-dialog').unblock();
            $dialog.dialog('close');
            $('#refresh_posts_browser').click();
        });
    }
    
    $(document).ready(function()
    {
        var $dialog = $('#schedule_editor');
        
        $('#schedule_date_control').datepicker({ dateFormat: 'yy-mm-dd' });
        
        $dialog.dialog({
            autoOpen: false,
            modal:    true,
            width:    330,
            buttons: [
                {
                    text:  $dialog.attr('data-ok-caption'),
                    icons: { primary: "ui-icon-check" },
                    click: function() { save_post_publishing_date(); }
                }, {
                    text:  $dialog.attr('data-cancel-caption'),
                    icons: { primary: "ui-icon-cancel" },
                    click: function() { $(this).dialog('close'); }
                }
            ]
        })
    });
</script>
