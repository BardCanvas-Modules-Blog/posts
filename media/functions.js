
var posts_category_selector_post_process_extensions;

var fill_post_form_extensions;
var reset_post_form_extensions;

if( typeof fill_post_form_extensions == 'undefined' )
    fill_post_form_extensions = {};

if( typeof reset_post_form_extensions == 'undefined' )
    reset_post_form_extensions = {};

var post_autosaver_enabled  = false;
var post_autosaver_interval = null;
var post_autosaver_working  = false;

var browser_position = 0;

function prepare_post_addition()
{
    var $workarea = $('#form_workarea');
    $workarea.find('.for_edition').hide();
    $workarea.find('.for_addition').show();
    
    var $form = $('#post_form');
    $form.find('.post_buttons button[data-save-type="save_draft"]').show();
    $form.find('.post_buttons button[data-save-type="save"]').hide();
    $form.find('.post_buttons button[data-save-type="publish"]').show();
    
    reset_post_form();
    show_post_form();
    update_category_selector();
}

function edit_post(id_post)
{
    var $workarea = $('#form_workarea');
    $workarea.find('.for_edition').show();
    $workarea.find('.for_addition').hide();
    
    var url    = $_FULL_ROOT_PATH + '/posts/scripts/get_as_json.php';
    var params = {
        'id_post': id_post,
        'wasuuup': wasuuup()
    };
    
    if( typeof GLOBAL_AJAX_ADDED_PARAMS !== 'undefined' )
        for(var i in GLOBAL_AJAX_ADDED_PARAMS)
            params[i] = GLOBAL_AJAX_ADDED_PARAMS[i];
    
    $.blockUI(blockUI_default_params);
    $.getJSON(url, params, function(data)
    {
        if( data.message != 'OK' )
        {
            $.unblockUI();
            alert(data.message);
            
            return;
        }
        
        var record = data.data;
        var $form  = $('#post_form');
        
        $form.find('.post_buttons button[data-save-type="save_draft"]').hide();
        $form.find('.post_buttons button[data-save-type="save"]').show();
        
        if( record.status == 'draft' || (record.status != 'published' && $_CURRENT_USER_IS_MOD) )
            $form.find('.post_buttons button[data-save-type="publish"]').show();
        else
            $form.find('.post_buttons button[data-save-type="publish"]').hide();
        
        reset_post_form();
        fill_post_form($form, record);
        $.unblockUI();
        show_post_form();
        update_category_selector(record.main_category);
    });
}

function copy_post(id_post)
{
    var $workarea = $('#form_workarea');
    $workarea.find('.for_edition').hide();
    $workarea.find('.for_addition').show();
    
    var url    = $_FULL_ROOT_PATH + '/posts/scripts/get_as_json.php';
    var params = {
        'id_post': id_post,
        'wasuuup': wasuuup()
    };
    
    if( typeof GLOBAL_AJAX_ADDED_PARAMS !== 'undefined' )
        for(var i in GLOBAL_AJAX_ADDED_PARAMS)
            params[i] = GLOBAL_AJAX_ADDED_PARAMS[i];
    
    $.blockUI(blockUI_default_params);
    $.getJSON(url, params, function(data)
    {
        if( data.message != 'OK' )
        {
            $.unblockUI();
            alert(data.message);
            
            return;
        }
        
        var record             = data.data;
        record.id_post         = '';
        record.slug            = '';
        record.status          = 'draft';
        record.publishing_date = '';
        record.expiration_date = '';
        record.author_level    = $_CURRENT_USER_LEVEL;
        
        var $form  = $('#post_form');
        
        reset_post_form();
        fill_post_form($form, record);
        $.unblockUI();
        show_post_form();
        update_category_selector(record.main_category);
    });
}

function add_child_post_of(id_parent_post)
{
    var $workarea = $('#form_workarea');
    $workarea.find('.for_edition').hide();
    $workarea.find('.for_addition').show();
    
    var url    = $_FULL_ROOT_PATH + '/posts/scripts/get_as_json.php';
    var params = {
        'id_post': id_parent_post,
        'wasuuup': wasuuup()
    };
    
    if( typeof GLOBAL_AJAX_ADDED_PARAMS !== 'undefined' )
        for(var i in GLOBAL_AJAX_ADDED_PARAMS)
            params[i] = GLOBAL_AJAX_ADDED_PARAMS[i];
    
    $.blockUI(blockUI_default_params);
    $.getJSON(url, params, function(data)
    {
        if( data.message != 'OK' )
        {
            $.unblockUI();
            alert(data.message);
            
            return;
        }
        
        var record = data.data;
        reset_post_form();
        var post_permalink = post_permalink_style == 'slug' ? record.slug : id_parent_post;
        var $form  = $('#post_form');
        $form.find('.parent_post_title').html(sprintf(
            '<a href="%s/%s" target="_blank">%s</a>',
            $_FULL_ROOT_PATH, post_permalink, record.title
        ));
        $form.find('.parent_post_area').show();
        $form.find('input[name="parent_post"]').val(id_parent_post);
        
        $.unblockUI();
        show_post_form();
        update_category_selector(record.main_category);
    });
}

function reset_post_form()
{
    var $form = $('#post_form');
    
    $form.find('#custom_fields_target').html('');
    $form[0].reset();
    
    $form.find('input[name="is_autosave"]').val('false');
    $form.find('input[name="id_post"]').val('');
    $form.find('input[name="status"]').val('draft');
    $form.find('.subfield select[name="visibility"] option:first').prop('selected', true);
    
    let $option = $form.find('.subfield select[name="visibility"] option[value="level_based"]');
    let caption = $option.attr('data-caption');
    $option.text(caption.replace('{$level}', $_CURRENT_USER_LEVEL));
    
    toggle_fa_pseudo_switch($form.find('.subfield .fa-pseudo-switch[data-input-name="allow_comments"]'), true);
    toggle_fa_pseudo_switch($form.find('.subfield .fa-pseudo-switch[data-input-name="pin_to_home"]'), false);
    toggle_fa_pseudo_switch($form.find('.subfield .fa-pseudo-switch[data-input-name="pin_to_main_category_index"]'), false);
    
    var thumbnail = $form.find('.subfield.featured_image .thumbnail img').attr('data-empty-src');
    $form.find('.subfield.featured_image .thumbnail img').attr('src', thumbnail);
    $form.find('input[name="id_featured_image"]').val('');
    
    for(var fi in reset_post_form_extensions)
        if( typeof reset_post_form_extensions[fi] == 'function' )
            reset_post_form_extensions[fi]($form);
    
    set_post_password('');
    prefill_scheduling_controls('');
    
    $form.find('.field[data-field="controls"]').hide();
    
    $form.find('input[name="parent_post"]').val('');
    $form.find('.parent_post_title').html('&mdash;');
    $form.find('.parent_post_area').hide();
    
    // $form.find('.post_buttons').unblock();
}

/**
 * 
 * @param {jQuery} $form
 * @param {object} record
 */
function fill_post_form($form, record)
{
    $form.find('input[name="id_post"]').val( record.id_post );
    $form.find('input[name="status"]').val( record.status );
    $form.find('textarea[name="title"]').val( record.title );
    $form.find('input[name="slug"]').val( record.slug );
    $form.find('textarea[name="excerpt"]').val( record.excerpt );
    
    if( record.publishing_date == '0000-00-00 00:00:00' ) record.publishing_date = '';
    if( record.expiration_date == '0000-00-00 00:00:00' ) record.expiration_date = '';
    prefill_scheduling_controls(record.publishing_date, record.expiration_date);
    
    set_post_password(record.password);
    
    let $option = $form.find('.subfield select[name="visibility"] option[value="' + record.visibility + '"]');
    $option.prop('selected', true);
    if( record.visibility === 'level_based' )
    {
        let caption = $option.attr('data-caption');
        $option.text(caption.replace('{$level}', record.author_level));
    }
    
    var allow_comments = parseInt(record.allow_comments) == 1;
    toggle_fa_pseudo_switch($form.find('.subfield .fa-pseudo-switch[data-input-name="allow_comments"]'), allow_comments);
    
    var pin_to_home = parseInt(record.pin_to_home) == 1;
    toggle_fa_pseudo_switch($form.find('.subfield .fa-pseudo-switch[data-input-name="pin_to_home"]'), pin_to_home);
    
    var pin_to_main_category_index = parseInt(record.pin_to_main_category_index) == 1;
    toggle_fa_pseudo_switch($form.find('.subfield .fa-pseudo-switch[data-input-name="pin_to_main_category_index"]'), pin_to_main_category_index);
    
    var thumbnail = record.featured_image_thumbnail == ''
        ? $form.find('.subfield.featured_image .thumbnail img').attr('data-empty-src')
        : record.featured_image_thumbnail;
    $form.find('.subfield.featured_image .thumbnail img').attr('src', thumbnail);
    $form.find('input[name="id_featured_image"]').val( record.id_featured_image );
    
    var editor = tinymce.get('post_content_editor');
    editor.setContent(record.content, {format : 'raw'});
    
    for(var fi in fill_post_form_extensions)
        if( typeof fill_post_form_extensions[fi] == 'function' )
            fill_post_form_extensions[fi]($form, record);
    
    $form.find('.field[data-field="controls"]').show();
    
    $form.find('input[name="parent_post"]').val(record.parent_post);
    if( record.parent_post == '' || record.parent_post == 0 )
    {
        $form.find('.parent_post_title').html('&mdash;');
        $form.find('.parent_post_area').hide();
    }
    else
    {
        var parent_permalink = post_permalink_style == 'slug' ? record.parent_post_slug : record.parent_post;
        $form.find('.parent_post_title').text(record.parent_post_title);
        $form.find('.parent_post_title').html(sprintf(
            '<a href="%s/%s" target="_blank">%s</a>',
            $_FULL_ROOT_PATH, parent_permalink, record.parent_post_title
        ));
        $form.find('.parent_post_area').show();
    }
    
    fill_custom_fields(record);
}

function update_category_selector(preselected_id)
{
    if( typeof preselected_id == 'undefined' ) preselected_id = '';
    
    var $container = $('#main_category_selector_container');
    $container.block(blockUI_smallest_params);
    
    if( typeof GLOBAL_AJAX_ADDED_PARAMS === 'undefined' )
        GLOBAL_AJAX_ADDED_PARAMS = {};
    
    var url = $_FULL_ROOT_PATH + '/categories/scripts/tree_as_json.php'
            + '?with_description=true'
            + '&wasuuup=' + wasuuup();
    $.getJSON(url, GLOBAL_AJAX_ADDED_PARAMS, function(data)
    {
        if( data.message != 'OK' )
        {
            alert(data.message);
            $container.unblock();
            
            return;
        }
        
        var $select = $container.find('select');
        $select.find('option').remove();
        
        var $exceptions = $('#category_exceptions');
        
        var selected;
        for( var key in data.data )
        {
            if( $exceptions.length > 0 )
                if( $exceptions.find('div[data-id-category="' + key + '"]').length > 0 )
                    continue;
            
            selected = key == preselected_id ? 'selected' : '';
            $select.append('<option ' + selected + ' value="' + key + '">' + data.data[key] + '</option>');
        }
        
        if( typeof posts_category_selector_post_process_extensions == 'object' )
            for( var i in posts_category_selector_post_process_extensions )
                posts_category_selector_post_process_extensions[i]($select);
        
        $container.unblock();
    });
}

function set_post_featured_image()
{
    var $form     = $('#post_form');
    var editor_id = $form.find('textarea[class*="tinymce"]').attr('id');
    var editor    = tinymce.get(editor_id);
    
    load_media_browser_in_tinymce_dialog(
        editor,
        $(window).width() - 20,
        $(window).height() - 60,
        'image',
        'parent.' + 'set_selected_gallery_image_as_featured_image'
    );
}

function set_selected_gallery_image_as_featured_image(
    id_media, type, file_url, thumbnail_url, width, height, embed_width
) {
    var $strings = $('#post_gallery_embed_strings');
    
    if( type != 'image' )
    {
        var message = $strings.find('.invalid_type_for_image').text();
        alert( message );
        
        return;
    }
    
    tinymce.activeEditor.windowManager.close();
    
    var $form = $('#post_form');
    $form.find('input[name="id_featured_image"]').val(id_media);
    $form.find('.subfield.featured_image .thumbnail img').attr('src', thumbnail_url);
}

function remove_post_featured_image()
{
    var $form     = $('#post_form');
    var empty_src = $form.find('.subfield.featured_image .thumbnail img').attr('data-empty-src');
    $form.find('input[name="id_featured_image"]').val('');
    $form.find('.subfield.featured_image .thumbnail img').attr('src', empty_src);
}

function trash_post(id_post)
{
    var url = $_FULL_ROOT_PATH + '/posts/scripts/trash.php';
    var params = {
        'id_post': id_post,
        'wasuuup': wasuuup()
    };
    
    var $row = $('#posts_browser_table').find('tr[data-record-id="' + id_post + '"]');
    
    $row.block(blockUI_smallest_params);
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert(response);
            $row.unblock();
            
            return;
        }
    
        $row.unblock();
        $('#refresh_posts_browser').click();
    });
}

function show_post_form()
{
    browser_position = $(window).scrollTop();
    
    hook_abandon_post();
    $('#main_workarea').hide('fast');
    $('#form_workarea').show('fast');
    start_post_autosaver();
    
    $.scrollTo(0, 250);
}

function hide_post_form()
{
    unhook_abandon_post();
    
    if( post_form_iframed_mode )
    {
        parent.postMessage('BCM:CLOSE_FRAME', '*');
        location.href = '/BCM/CLOSE_FRAME';
        
        return;
    }
    
    $('#form_workarea').hide('fast');
    $('#main_workarea').show('fast', function() { $.scrollTo(browser_position, 250); });
}

function prepare_post_preview()
{
    var $form = $('#post_form');
    $form.find('input[name=status]').val('draft');
    $form.attr('data-prevew-mode', 'true');
    $form.find('input[name="is_preview"]').val('true');
}

function prepare_post_form_serialization()
{
    var $form = $('#post_form');
    
    if( $form.find('input[name="is_autosave"]').val() != 'true' )
        if( $form.find('.field[data-field="publishing_date"] .input .controls .fieldset').is(':visible') )
            set_schedule_date();
    
    $form.find('textarea[class*="tinymce"]').each(function()
    {
        var id      = $(this).attr('id');
        var editor  = tinymce.get(id);
        var content = editor.getContent();
        $(this).val( content );
    });
}

function prepare_post_form_submission()
{
    if( ! post_autosaver_working ) $.blockUI(blockUI_default_params);
}

function process_post_form_response(response, status, xhr, $form)
{
    if( ! post_autosaver_working ) $.unblockUI();
    
    if( response.indexOf('OK') < 0 )
    {
        if( post_autosaver_working )
        {
            $form.find('.post_autosave_status .saving').hide();
            $form.find('.post_autosave_status .saved').hide();
            $form.find('.post_autosave_status .error').show();
            $form.find('.post_autosave_status .error .message').text( response );
            $form.find('.post_buttons button:visible').prop('disabled', false);
            $form.find('input[name="is_autosave"]').val('false');
            console.log('Draft autosave finished.');
            
            post_autosaver_working = false;
            return;
        }
        
        alert( response );
        return;
    }
    
    var id_post = response.replace('OK:', '');
    if( parseInt(id_post) === 0 ) id_post = '';
    if( id_post != '' ) $form.find('input[name="id_post"]').val( id_post );
    
    if( post_autosaver_working )
    {
        $form.find('.post_autosave_status .saving').hide();
        $form.find('.post_autosave_status .error').hide();
        $form.find('.post_autosave_status .saved').show();
        $form.find('.post_buttons button:visible').prop('disabled', false);
        $form.find('input[name="is_autosave"]').val('false');
        console.log('Draft autosave finished.');
        
        post_autosaver_working = false;
        return;
    }
    
    if( $form.attr('data-prevew-mode') == 'true' )
    {
        $form.attr('data-prevew-mode', '');
        $form.find('input[name="is_preview"]').val('false');
        
        var url = $_FULL_ROOT_PATH + '/' + id_post + '?preview=true&wasuuup=' + wasuuup();
        window.open(url, 'post_preview_' + id_post).focus();
        return;
    }
    
    stop_post_autosaver();
    hide_post_form();
    $('#refresh_posts_browser').click();
}

function start_post_autosaver()
{
    if( post_autosaver_interval ) return;
    
    var $form = $('#post_form');
    $form.find('.post_autosave_status .saving').hide();
    $form.find('.post_autosave_status .error').hide();
    $form.find('.post_autosave_status .saved').hide();
    $form.find('.post_buttons button:visible').prop('disabled', false);
    
    if( $form.find('input[name="status"]').val() != 'draft' ) return;
    
    post_autosaver_enabled = true;
    post_autosaver_interval = setInterval('autosave_post()', post_autosaver_heartbit);
    console.log('Draft autosaver started');
}

function stop_post_autosaver()
{
    post_autosaver_enabled = false;
    clearInterval(post_autosaver_interval);
    
    var $form = $('#post_form');
    $form.find('.post_autosave_status .saving').hide();
    $form.find('.post_autosave_status .error').hide();
    $form.find('.post_autosave_status .saved').hide();
    $form.find('.post_buttons button:visible').prop('disabled', false);
    
    console.log('Draft autosaver stopped');
}

function autosave_post()
{
    if( ! post_autosaver_enabled ) return;
    if( post_autosaver_working ) return;
    
    post_autosaver_working = true;
    
    var $form   = $('#post_form');
    var title   = $.trim($form.find('textarea[name="title"]').val());
    var content = $.trim(tinymce.get('post_content_editor').getContent());
    
    if( title == '' || content == '' )
    {
        post_autosaver_working = false;
        return;
    }
    
    console.log('Starting draft autosave...');
    $form.find('.post_buttons button:visible').prop('disabled', true);
    $form.find('.post_autosave_status .saving').show();
    $form.find('.post_autosave_status .saved').hide();
    $form.find('.post_autosave_status .error').hide();
    $form.find('input[name="is_autosave"]').val('true');
    $form.submit();
}

function edit_post_password()
{
    var $form   = $('#post_form');
    var $field  = $form.find('input[name="password"]');
    var value   = $field.val();
    var message = $field.attr('placeholder');
    
    var res = prompt(message, value);
    if( res == null ) return;
    
    set_post_password(res);
}

function set_post_password(value)
{
    var $form  = $('#post_form');
    var $field = $form.find('input[name="password"]');
    var $dummy = $form.find('.password_control .current_password');
    
    $field.val( value );
    
    if( value != '' )
    {
        $dummy.text(value);
        $dummy.toggleClass('state_active', true);
        $form.find('.password_control .remove').show();
    }
    else
    {
        var placeholder = $dummy.attr('data-none-caption');
        $dummy.text(placeholder);
        $dummy.toggleClass('state_active', false);
        $form.find('.password_control .remove').hide();
    }
}

function emancipate_post(post_id, trigger, callback)
{
    if( ! confirm($_GENERIC_CONFIRMATION) ) return;
    
    var $trigger = $(trigger);
    var url      = $_FULL_ROOT_PATH + '/posts/scripts/toolbox.php';
    var params   = {
        action:  'remove_parent',
        id_post: post_id,
        wasuuup: wasuuup()
    };
    
    $trigger.block(blockUI_smallest_params);
    $.get(url, params, function(response)
    {
        $trigger.unblock();
        if( response != 'OK' )
        {
            alert(response);
            
            return;
        }
        
        $trigger.fadeOut('fast', function()
        {
            $(this).remove();
            
            if( typeof callback == 'function' ) callback();
        });
    });
}

function fill_custom_fields(record)
{
    if( typeof record.custom_fields == 'undefined' ) return;
    
    var $template = $('#custom_field_template');
    if( $template.length == 0 ) return;
    
    var html    = $template.html();
    var $target = $('#custom_fields_target');
    for( var i in record.custom_fields )
    {
        var fname = i;
        var fval  = record.custom_fields[i];
        
        var $node = $(html);
        $node.find('input[type="text"]').val( fname );
        $node.find('textarea').val(fval);
        $node.appendTo($target);
        
        $target.find('textarea.new').expandingTextArea();
        $target.find('textarea.new').removeClass('new');
    }
}

function add_custom_field()
{
    var $template = $('#custom_field_template');
    if( $template.length == 0 ) return;
    
    var html    = $template.html();
    var $target = $('#custom_fields_target');
    $target.append(html);
    $target.find('textarea.new').expandingTextArea();
    $target.find('textarea.new').removeClass('new');
}

function prefill_scheduling_controls(preset_date, expiration_date)
{
    if( typeof expiration_date == 'undefined' ) expiration_date = '';
    
    var $form      = $('#post_form');
    var $container = $form.find('.field[data-field="publishing_date"]');
    var $field     = $container.find('input[name="publishing_date"]');
    var status     = $form.find('input[name="status"]').val();
    
    $field.val(preset_date);
    if( preset_date == '' )
    {
        $container.find('.input .values .specific').html('&mdash;');
        $container.find('.input .values .automatic').show();
        $container.find('.input .values .specific').hide();
    }
    else
    {
        $container.find('.input .values .specific').text(preset_date);
        $container.find('.input .values .automatic').hide();
        $container.find('.input .values .specific').show();
    }
    
    hide_scheduling_controls();
    if( status == 'published' || expiration_date != '' )
    {
        $container.find('.input .controls .trigger').hide();
        $container.find('.input .controls .fieldset').hide();
    }
    else
    {
        $container.find('.input .controls .trigger').show();
        $container.find('.input .controls .fieldset').hide();
    }
}

function show_scheduling_controls(preset_date)
{
    var $form      = $('#post_form');
    var $container = $form.find('.field[data-field="publishing_date"]');
    var $field     = $container.find('input[name="publishing_date"]');
    
    if( typeof preset_date == 'undefined' )
    {
        var field_value = $field.val();
        if( field_value != '' )
        {
            preset_date = field_value;
        }
        else
        {
            var dateObject = new Date();
            dateObject.setHours(dateObject.getHours() + 1);
            preset_date = moment(dateObject).format("YYYY-MM-DD HH:mm:ss");
        }
    }
    
    $container.find('.input .values').hide();
    $container.find('.input .controls .trigger').hide();
    $container.find('.input .controls .fieldset').show();
    
    // console.log( preset_date );
    var _parts  = preset_date.split(' ');
    var _date   = _parts[0];
    var _time   = _parts[1].substring(5, 0);
    var val;
    
    val = $container.find('input[name="schedule[date]"]').val();
    $container.find('input[name="schedule[date]"]').attr('data-original-value', val);
    $container.find('input[name="schedule[date]"]').val(_date);
    
    val = $container.find('input[name="schedule[time]"]').val();
    $container.find('input[name="schedule[time]"]').attr('data-original-value', val);
    $container.find('input[name="schedule[time]"]').val(_time);
    // $form.find('.post_buttons').fadeOut('fast');
}

function cancel_schedule_edition()
{
    hide_scheduling_controls();
    
    var $container = $('#post_form').find('.field[data-field="publishing_date"]');
    var val;
    
    val = $container.find('input[name="schedule[date]"]').attr('data-original-value');
    $container.find('input[name="schedule[date]"]').val(val);
    
    val = $container.find('input[name="schedule[time]"]').attr('data-original-value');
    $container.find('input[name="schedule[time]"]').val(val);
}

function remove_automatic_schedule()
{
    var $container = $('#post_form').find('.field[data-field="publishing_date"]');
    
    $container.find('input[name="publishing_date"]').val('');
    $container.find('.values .specific').html('&mdash;');
    $container.find('.values .specific').hide();
    $container.find('.values .automatic').show();
    hide_scheduling_controls();
}

function hide_scheduling_controls()
{
    var $form      = $('#post_form');
    var $container = $form.find('.field[data-field="publishing_date"]');
    $container.find('.input .controls .fieldset').hide();
    $container.find('.input .controls .trigger').show();
    $container.find('.input .values').show();
    
    // $form.find('.post_buttons').fadeIn('fast');
}

function set_schedule_date()
{
    var $container = $('#post_form').find('.field[data-field="publishing_date"]');
    var value = $container.find('input[name="schedule[date]"]').val()
              + ' '
              + $container.find('input[name="schedule[time]"]').val()
              + ':00'
    ;
    
    $container.find('input[name="publishing_date"]').val(value);
    $container.find('.values .specific').text(value);
    $container.find('.values .automatic').hide();
    $container.find('.values .specific').show();
    hide_scheduling_controls();
}

$(document).ready(function()
{
    $('#post_form').ajaxForm({
        target:          '#post_form_target',
        beforeSerialize: prepare_post_form_serialization,
        beforeSubmit:    prepare_post_form_submission,
        success:         process_post_form_response
    });
    
    $('#schedule_date').datepicker({
        showOn:          'button',
        buttonImage:     $_FULL_ROOT_PATH + '/media/icons/calendar.png',
        buttonImageOnly: true,
        dateFormat:      'yy-mm-dd'
    });
});
