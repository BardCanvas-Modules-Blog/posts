
// This definition should be set if used
// if( typeof $_POST_ADDON_FUNCTIONS == 'undefined' )
//     var $_POST_ADDON_FUNCTIONS = {};

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
