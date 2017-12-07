
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

function prepare_post_form_submission(formData, $form, options)
{
    $('#post_form').block(blockUI_big_progress_params);
}

function process_post_form_response(response)
{
    blockUI_progress_complete();
    
    var $form = $('#post_form');
    
    if( response.indexOf('OK:') < 0 )
    {
        $form.unblock();
        
        alert( response );
        return;
    }
    
    unhook_abandon_post();
    
    if( $form.find('input[name="as_popup"]').val() === 'true' )
    {
        $('#quick_post_form_container').html(sprintf('<p>%s</p>', saved_as_popup_message));
        if(parent) parent.postMessage('BARDCANVAS:CLOSE_CHILD_FRAME', '*');
        
        return;
    }
    
    var url    = response.replace('OK:', '');
    var status = $form.find('input[name="status"]').val();
    
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
        beforeSend:      blockUI_progress_init,
        uploadProgress:  blockUI_progress_update,
        success:         process_post_form_response
    });
    
    $form.find('.expandible_textarea').expandingTextArea();
});
