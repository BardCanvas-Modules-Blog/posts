
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
    $('#post_form').block(blockUI_medium_params);
}

function process_post_form_response(response)
{
    var $form = $('#post_form');
    
    if( response.indexOf('OK:') < 0 )
    {
        $form.unblock();
        
        alert( response );
        return;
    }
    
    var url    = response.replace('OK:', '');
    var status = $form.find('input[name="status"]').val();
    
    if( status == 'draft' ) location.reload();
    else                    location.href = url;
}

function add_attachment_to_quick_post(type, icon)
{
    var html  = '<div class="attachment framed_content state_highlight clearfix">'
              +     '<span class="fa pseudo_link fa-trash fa-fw pull-right" onclick="remove_attachment_from_quick_post(this)"></span>'
              +     sprintf('<span class="fa %s fa-fw"></span> ', icon)
              +     sprintf('<input type="file" name="attachments[%1$s][]" accept="%1$s/*"> ', type)
              + '</div>';
    
    $('#post_form').find('.field.attachments').append(html);
}

function remove_attachment_from_quick_post(trigger)
{
    $(trigger).closest('.attachment').fadeOut('fast', function() { $(this).remove(); });
}

$(document).ready(function()
{
    var $form = $('#post_form');
    $form.ajaxForm({
        target:       '#post_form_target',
        beforeSerialize: prepare_post_form_serialization,
        beforeSubmit:    prepare_post_form_submission,
        success:         process_post_form_response
    });
    
    $form.find('.expandible_textarea').expandingTextArea();
});
