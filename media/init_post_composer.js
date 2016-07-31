
// Important: these need to be redefined once the document is loaded.
var post_composer_tinymce_defaults = {};
    

$(document).ready(function()
{
    post_composer_tinymce_defaults = $.extend({}, tinymce_defaults);
    
    post_composer_tinymce_defaults.toolbar  = 'bold italic strikethrough forecolor fontsizeselect removeformat | alignleft aligncenter alignright | bullist numlist outdent indent | link | fullscreen';
    post_composer_tinymce_defaults.selector = '.tinymce_post_composer';
    
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
