<?php
/**
 * Styles and scripts for the module
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *
 * @var config   $config
 * @var module   $this_module
 * @var template $template;
 */

use hng2_base\config;
use hng2_base\module;
use hng2_base\template;
?>

<!-- posts global styles and functions -->

<!--suppress CssUnusedSymbol -->
<style type="text/css">
    #left_sidebar .group.archive_tree .item.month.collapsed, 
    #left_sidebar .group.archive_tree .item.year.expanded  .toggle.right ,
    #left_sidebar .group.archive_tree .item.year.collapsed .toggle.down  {
        display: none;
    }
    
    .category_messages .message {
        font-family: arial, helvetica, sans-serif;
        font-size: 10pt;
        line-height: 12pt;
        margin: 5px 0 0 0;
    }
    
    #quick_post_floating_trigger {
        color: darkgreen;
        background-color: greenyellow;
        -webkit-box-shadow: 2px 2px 10px 1px rgba(0, 0, 0, 1);
        -moz-box-shadow:    2px 2px 10px 1px rgba(0, 0, 0, 1);
        box-shadow:         2px 2px 10px 1px rgba(0, 0, 0, 1);
        width: 42px; height: 42px; font-size: 32px; padding: 5px;
        text-align: center;
        border-radius: 100%;
        display: inline-block;
        cursor: pointer;
        position: fixed; bottom: 10px; left: 10px;
        z-index: 100;
    }
    
    #quick_post_floating_trigger:hover { color: black; }
</style>

<script type="text/javascript">
    var abandon_post_question = '<?= $this_module->language->messages->abandon_post_question ?>';
</script>
<script type="text/javascript" 
        src="<?= $config->full_root_path ?>/posts/media/global_functions~v<?= $config->scripts_version ?>.js"></script>

<script type="text/javascript">
    
    $(document).ready(function()
    {
        if( ! $_IS_MOBILE ) return;
        if( $_CURRENT_USER_ID_ACCOUNT == '' ) return;
        if( $_PHP_SELF == '/posts/quick_single.php' ) return;
        
        var $trigger = $(sprintf(
            '<a id="quick_post_floating_trigger" class="fa fa-bolt fa-fw"  href="%s" style="display: none; text-decoration: none;"></a>',
            $_FULL_ROOT_PATH + '/posts/quick_single.php'
        ));
        
        /*
        $trigger.click(function()
        {
            $form = $('#quick_post_form_container');
            if( $form.length > 0 )
            {
                expand_quick_post_area();
                $.scrollTo($form, 1000);
                $(this).fadeOut('fast', function() { $(this).remove(); });
            }
            else
            {
                location.href = $_FULL_ROOT_PATH + '/posts/quick_single.php';
            }
        });
        */
        
        $('body').append( $trigger );
        $trigger.fadeIn('fast');
    });
</script>
