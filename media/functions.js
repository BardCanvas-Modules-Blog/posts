
function reset_filter()
{
    var $form = $('#filter_form');
    $form.find('input[name="search_for"]').val('');
    $form.find('select[name="limit"] options:first').prop('selected', true);
    $form.find('input[name="offset"]').val('0');
    $form.find('input[name="order"]').val('3');
}

//noinspection JSUnusedGlobalSymbols
function paginate(value)
{
    var $form = $('#filter_form');
    $form.find('input[name="offset"]').val(value);
    $form.submit();
}

function prepare_post_addition()
{
    $('#form_workarea')
        .find('.for_edition').hide()
        .find('.for_addition').show()
    ;
    reset_post_form();
    show_post_form();
}

function edit_post(id_post)
{
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
        
        reset_post_form();
        fill_post_form($form, record);
        $.unblockUI();
        show_post_form();
    });
}

function copy_post(id_post)
{
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
    });
}

/**
 * 
 * @param {jQuery} $form
 * @param {object} record
 */
function fill_post_form($form, record)
{
    //TODO: Implement fill_post_form() method
}

function delete_post(id_post)
{
    var url = $_FULL_ROOT_PATH + '/posts/scripts/delete.php';
    var params = {
        'id_post': id_post,
        'wasuuup': parseInt(Math.random() * 1000000000000000)
    };
    
    $.blockUI(blockUI_smallest_params);
    $.get(url, params, function(response)
    {
        if( response != 'OK' )
        {
            alert(response);
            $.unblockUI();
            
            return;
        }
    
        $.unblockUI();
        $('#refresh_post_browser').click();
    });
}

function reset_post_form()
{
    var $form = $('#post_form');
    $form[0].reset();
    $form.find('.field[data-field="excerpt"]').hide();
    $form.find('.post_addons_bar .post_addon[data-related-field="excerpt"]').show();
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
