
var fill_post_form_extensions;
var reset_post_form_extensions;

if( typeof fill_post_form_extensions == 'undefined' )
    fill_post_form_extensions = {};

if( typeof reset_post_form_extensions == 'undefined' )
    reset_post_form_extensions = {};

var post_autosaver_enabled  = false;
var post_autosaver_interval = null;
var post_autosaver_working  = false;

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
        'wasuuup': parseInt(Math.random() * 1000000000000000)
    };
    
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
        'wasuuup': parseInt(Math.random() * 1000000000000000)
    };
    
    $.blockUI(blockUI_default_params);
    $.getJSON(url, params, function(data)
    {
        if( data.message != 'OK' )
        {
            $.unblockUI();
            alert(data.message);
            
            return;
        }
        
        var record         = data.data;
        record.id_post     = '';
        record.slug        = '';
        
        var $form  = $('#post_form');
        
        reset_post_form();
        fill_post_form($form, record);
        $.unblockUI();
        show_post_form();
        update_category_selector(record.main_category);
    });
}

function reset_post_form()
{
    var $form = $('#post_form');
    
    $form[0].reset();
    
    $form.find('input[name="id_post"]').val('');
    $form.find('input[name="slug"]').data('modified', false);
    
    $form.find('.subfield select[name="visibility"] option:first').prop('selected', true);
    
    toggle_fa_pseudo_switch($form.find('.subfield .fa-pseudo-switch[data-input-name="allow_comments"]'), true);
    toggle_fa_pseudo_switch($form.find('.subfield .fa-pseudo-switch[data-input-name="pin_to_home"]'), false);
    toggle_fa_pseudo_switch($form.find('.subfield .fa-pseudo-switch[data-input-name="pin_to_main_category_index"]'), false);
    
    var thumbnail = $form.find('.subfield.featured_image .thumbnail img').attr('data-empty-src');
    $form.find('.subfield.featured_image .thumbnail img').attr('src', thumbnail);
    $form.find('input[name="id_featured_image"]').val('');
    
    for(var fi in reset_post_form_extensions)
        if( typeof reset_post_form_extensions[fi] == 'function' )
            reset_post_form_extensions[fi]($form);
    
    $form.find('.field[data-field="controls"]').hide();
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
    $form.find('textarea[name="excerpt"]').val( record.excerpt );
    
    $form.find('input[name="password"]').val( record.password );
    
    $form.find('.subfield select[name="visibility"] option[value="' + record.visibility + '"]').prop('selected', true);
    
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
}

function update_category_selector(preselected_id)
{
    if( typeof preselected_id == 'undefined' ) preselected_id = '';
    
    var $container = $('#main_category_selector_container');
    $container.block(blockUI_smallest_params);
    
    var url = $_FULL_ROOT_PATH + '/categories/scripts/tree_as_json.php'
            + '?with_description=true'
            + '&wasuuup=' + parseInt(Math.random() * 1000000000000000);
    $.getJSON(url, function(data)
    {
        if( data.message != 'OK' )
        {
            alert(data.message);
            $container.unblock();
            
            return;
        }
        
        var $select = $container.find('select');
        $select.find('option').remove();
        
        var selected;
        for( var key in data.data )
        {
            selected = key == preselected_id ? 'selected' : '';
            $select.append('<option ' + selected + ' value="' + key + '">' + data.data[key] + '</option>');
        }
        
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
        'top.' + 'set_selected_gallery_image_as_featured_image'
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
    
    top.tinymce.activeEditor.windowManager.close();
    
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
        'wasuuup': parseInt(Math.random() * 1000000000000000)
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
    $('#main_workarea').hide('fast');
    $('#form_workarea').show('fast');
    start_post_autosaver();
}

function hide_post_form()
{
    $('#form_workarea').hide('fast');
    $('#main_workarea').show('fast');
}

function update_slug()
{
    var $form = $('#post_form');
    if( $form.find('input[name="id_post"]').val() != '' ) return;
    if( $form.find('input[name="slug"]').data('modified') ) return;
    
    var title = $form.find('input[name="title"]');
    var slug  = title.toLowerCase();
    
    slug = slug.replace(/[^a-z0-9\-_]/g, "-");
    slug = slug.replace(/\-+/g, "-");
    slug = slug.replace(/_+/g, "_");
    
    $form.find('input[name="slug"]').val(slug);
}

function prepare_post_form_serialization()
{
    var $form = $('#post_form');
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
            console.log('Draft autosave finished.');
    
            post_autosaver_working = false;
            return;
        }
        
        alert( response );
        return;
    }
    
    id_post = response.replace('OK:', '');
    if( id_post != '' ) $form.find('input[name="id_post"]').val( id_post );
    
    if( post_autosaver_working )
    {
        $form.find('.post_autosave_status .saving').hide();
        $form.find('.post_autosave_status .error').hide();
        $form.find('.post_autosave_status .saved').show();
        $form.find('.post_buttons button:visible').prop('disabled', false);
        console.log('Draft autosave finished.');
        
        post_autosaver_working = false;
        return;
    }
    
    if( $form.attr('data-prevew-mode') == 'true' )
    {
        $form.attr('data-prevew-mode', '');
        
        var url = $_FULL_ROOT_PATH + '/' + id_post + '?preview=true&wasuuup=' + parseInt(Math.random() * 1000000000000000);
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
    $form.submit();
}

$(document).ready(function()
{
    $('#post_form').ajaxForm({
        target:       '#post_form_target',
        beforeSerialize: prepare_post_form_serialization,
        beforeSubmit:    prepare_post_form_submission,
        success:         process_post_form_response
    });
});
