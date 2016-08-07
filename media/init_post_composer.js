
// Important: these need to be redefined once the document is loaded.
var post_composer_tinymce_defaults = {};
    
$(document).ready(function()
{
    post_composer_tinymce_defaults = $.extend({}, tinymce_defaults);
    
    post_composer_tinymce_defaults.toolbar  = tinymce_full_toolbar;
    post_composer_tinymce_defaults.selector = '.tinymce_post_composer';
    
    if( tinymce_custom_toolbar_buttons.length > 0 )
        post_composer_tinymce_defaults.toolbar = post_composer_tinymce_defaults.toolbar + ' | ' + tinymce_custom_toolbar_buttons.join(' ');
    post_composer_tinymce_defaults.toolbar = post_composer_tinymce_defaults.toolbar  + ' | fullscreen';
    
    if( $_CURRENT_USER_IS_ADMIN )
    {
        post_composer_tinymce_defaults.toolbar
            = post_composer_tinymce_defaults.toolbar + ' | code';
        
        post_composer_tinymce_defaults.contextmenu
            = post_composer_tinymce_defaults.contextmenu + ' inserttable | cell row column deletetable';
    }
    
    if( $_CURRENT_USER_LANGUAGE != "en" && $_CURRENT_USER_LANGUAGE != "en_US" )
        post_composer_tinymce_defaults.language = $_CURRENT_USER_LANGUAGE;
});
