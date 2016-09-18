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
</style>

<script type="text/javascript">
    
    var abandon_post_question = '<?= $this_module->language->messages->abandon_post_question ?>';
    
    function hook_abandon_post()
    {
        $(window).bind('beforeunload.posts', function()
        {
            return abandon_post_question;
        });
    }
    
    function unhook_abandon_post()
    {
        $(window).unbind('beforeunload.posts');
    }
    
    $(document).ready(function()
    {
        $('#left_sidebar').find('.group.archive_tree .item.year').click(function()
        {
            var $parent = $(this).closest('.group');
            var year    = $(this).attr('data-year');
            
            $parent.find('.item[data-year="' + year + '"]').toggleClass('collapsed expanded');
        });
    });
    
    function remove_tag_from_post(trigger, id_post, tag)
    {
        var $trigger = $(trigger);
        
        var url = $_FULL_ROOT_PATH
                + '/posts/scripts/remove_tag.php'
                + '?id_post=' + id_post
                + '&tag=' + tag
                + '&wasuuup=' + wasuuup()
                ;
        $trigger.block(blockUI_smallest_params);
        $.get(url, function(response)
        {
            $trigger.unblock();
            if( response != 'OK' )
            {
                alert(response);
                
                return;
            }
            
            $trigger.remove();
        });
    }
</script>
