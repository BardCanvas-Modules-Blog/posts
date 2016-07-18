
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
        
        if( record.status == 'draft' )
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
    
    var editor = tinymce.get('post_content_editor');
    editor.setContent(record.content, {format : 'raw'});
    
    if( record.excerpt.length > 0 )
    {
        $form.find('textarea[name="excerpt"]').val( record.excerpt );
        $form.find('.field[data-field="excerpt"]').show();
        $form.find('.post_addons_bar[data-related-field="excerpt"]').hide();
    }
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

function reset_post_form()
{
    var $form = $('#post_form');
    $form[0].reset();
    $form.find('.field[data-field="excerpt"]').hide();
    $form.find('.post_addons_bar .post_addon[data-related-field="excerpt"]').show();
    $form.find('input[name="id_post"]').val('');
    $form.find('input[name="slug"]').data('modified', false);
}

function show_post_form()
{
    $('#main_workarea').hide('fast');
    $('#form_workarea').show('fast');
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
    $form.find('textarea.tinymce').each(function()
    {
        var id      = $(this).attr('id');
        var editor  = tinymce.get(id);
        var content = editor.getContent();
        console.log( content );
        $(this).val( content );
    });
}

function prepare_post_form_submission()
{
    $.blockUI(blockUI_default_params);
}

function process_post_form_response(response)
{
    $.unblockUI();
    if( response != 'OK' )
    {
        alert( response );
        return;
    }
    
    hide_post_form();
    $('#refresh_posts_browser').click();
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
