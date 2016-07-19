
if( typeof $_POST_ADDON_FUNCTIONS == 'undefined' )
    var $_POST_ADDON_FUNCTIONS = {};

$_POST_ADDON_FUNCTIONS['toggle_post_excerpt_textarea'] = function($trigger, $form)
{
    $form
        .find('.field[data-field="excerpt"]')
        .show('fast', function()
        {
            $trigger.fadeOut('fast');
        });
};

$_POST_ADDON_FUNCTIONS['insert_gallery_image_in_post_editor'] = function($trigger, $form)
{
    var editor_id = $form.find('textarea.tinymce').attr('id');
    var editor    = tinymce.get(editor_id);
    
    load_media_browser_in_tinymce_dialog(editor, $(window).width() - 20, $(window).height() - 60, 'image');
};

/*
$_POST_ADDON_FUNCTIONS['embed_youtube_link_in_post_editor'] = function($trigger, $form)
{
    var $messages = $('#post_composer_messages');
    var _title    = $messages.find('.tinymce .youtube_embedder .title').text();
    var _caption  = $messages.find('.tinymce .youtube_embedder .caption').text();
    var _invalid  = $messages.find('.tinymce .youtube_embedder .invalid_link').text();
    
    var editor_id = $form.find('textarea.tinymce').attr('id');
    var editor    = tinymce.get(editor_id);
    editor.windowManager.open({
        title: _title,
        body: [
            {type: 'textbox', name: 'yt_link', label: _caption}
        ],
        onsubmit: function(e) {
            var link = e.data.yt_link;
            if( link.match(/^((https:\/\/)?(www\.)?youtube\.com\/watch\?v=.*)|((https:\/\/)?youtu\.be\/.*)/i) == null )
            {
                alert( _invalid );
                return;
            }
            
            if( link.match(/^https:\/\//i) == null ) link = 'https://' + link;
            
            editor.insertContent(
                '<a class="youtube_link" href="' + link + '">' + link + '</a>'
            );
        }
    });
};
*/

/**
 * Triggers the addon function tied to the form addon button being clicked
 *
 * @param src
 */
function trigger_post_form_addon(src)
{
    var $this = $(src);
    var function_to_call = $this.attr('data-function');
    var $form = $this.closest('form');
    
    if( typeof $_POST_ADDON_FUNCTIONS[function_to_call] == 'undefined' )
    {
        alert(
            'JavaScript Exception hit!\n\n' +
            'function "' + function_to_call + '" is undefined\n\n' +
            'Please contact the webmaster.'
        );
        
        return;
    }
    
    $_POST_ADDON_FUNCTIONS[function_to_call]($this, $form);
}
