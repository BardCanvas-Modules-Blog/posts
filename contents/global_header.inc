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

<link rel="stylesheet" href="<?= $this_module->get_url() ?>/media/global_styles~v1.1.0-26.css">
<script type="text/javascript">
    var abandon_post_question = '<?= $this_module->language->messages->abandon_post_question ?>';
    var toggle_post_category_message_callbacks;
</script>
<script type="text/javascript" src="<?= $this_module->get_url() ?>/media/global_functions~v1.3.0-4.js"></script>

<?
#
# Posts by tag mod helper
#

if($account->level < $config::MODERATOR_USER_LEVEL) return;

$remove_link_caption = $this_module->language->posts_by_tag_helper->remove_link;
$remove_link_message = htmlspecialchars($this_module->language->posts_by_tag_helper->message, ENT_QUOTES);
?>
<script type="text/javascript">
    $(document).ready(function()
    {
        var $containers = $('#right_sidebar').find('.widget[data-type="posts_by_tag"]');
        if( $containers.length == 0 ) return;
        
        $containers.find('li').each(function()
        {
            var $li     = $(this);
            var id_post = $li.closest('li').attr('data-post-id');
            var tag     = $li.closest('.posts_by_tag').attr('data-tag');
            var cookie  = $.cookie('posts_by_tag_removed_' + id_post + '_' + tag);
            if( cookie ) return;
            
            var $span = $('<span class="pseudo_link"><i class="fa fa-times"></i> <?= $remove_link_caption ?></span>');
            $span.click(function()
            {
                if( ! confirm($_GENERIC_CONFIRMATION) ) return;
                
                var $self   = $(this);
                var id_post = $self.closest('li').attr('data-post-id');
                var tag     = $self.closest('.posts_by_tag').attr('data-tag');
                
                remove_tag_from_post(
                    $self, id_post, tag,
                    function() {
                        throw_notification('<?= $remove_link_message ?>', 'success');
                        $.cookie('posts_by_tag_removed_' + id_post + '_' + tag, true, {expires: 1, path: '/'})
                    }
                );
            });
            
            $li.append('<br>');
            $li.append($span);
        });
    });
</script>
