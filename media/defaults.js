
var post_composer_tinymce_defaults = {
    menubar:                  false,
    statusbar:                false,
    selector:                 '.tinymce',
    plugins:                  'autoresize advlist contextmenu autolink lists link image anchor searchreplace table paste codemirror textcolor',
    toolbar:                  'bold italic strikethrough forecolor fontsizeselect removeformat | alignleft aligncenter alignright | bullist numlist outdent indent | link image',
    contextmenu:              'cut copy paste | link image',
    fontsize_formats:         '10pt 12pt 14pt 18pt 24pt 36pt',
    content_css:              $_TEMPLATE_URL + '/media/styles~v' + $_SCRIPTS_VERSION + '.css',
    autoresize_bottom_margin: 0,
    autoresize_min_height:    200,
    codemirror: {
        indentOnInit: true,
        config: {
            mode: 'htmlmixed',
            lineNumbers: true,
            lineWrapping: true,
            indentUnit: 1,
            tabSize: 1,
            matchBrackets: true,
            styleActiveLine: true
        },
        jsFiles: [
            'lib/codemirror.js',
            'addon/edit/matchbrackets.js',
            'mode/xml/xml.js',
            'mode/javascript/javascript.js',
            'mode/css/css.js',
            'mode/htmlmixed/htmlmixed.js',
            'addon/dialog/dialog.js',
            'addon/search/searchcursor.js',
            'addon/search/search.js',
            'addon/selection/active-line.js'
        ],
        cssFiles: [
            'lib/codemirror.css',
            'addon/dialog/dialog.css'
        ]
    }
};

if( $_CURRENT_USER_IS_ADMIN )
{
    post_composer_tinymce_defaults.toolbar
        = post_composer_tinymce_defaults.toolbar + ' | code';
    
    post_composer_tinymce_defaults.contextmenu
        = post_composer_tinymce_defaults.contextmenu + ' inserttable | cell row column deletetable';
}

if( $_CURRENT_USER_LANGUAGE != "en" && $_CURRENT_USER_LANGUAGE != "en_US" )
    post_composer_tinymce_defaults.language = $_CURRENT_USER_LANGUAGE;
