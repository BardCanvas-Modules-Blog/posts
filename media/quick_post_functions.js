
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
    
    unhook_abandon_post();
    if( status == 'draft' ) location.href = $_REQUEST_URI;
    else                    location.href = url;
}

$(document).ready(function()
{
    var $form = $('#post_form');
    $form.ajaxForm({
        target:          '#post_form_target',
        beforeSerialize: prepare_post_form_serialization,
        beforeSubmit:    prepare_post_form_submission,
        success:         process_post_form_response
    });
    
    $form.find('.expandible_textarea').expandingTextArea();
});
