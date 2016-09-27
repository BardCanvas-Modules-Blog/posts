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
</style>

<script type="text/javascript">
    var abandon_post_question = '<?= $this_module->language->messages->abandon_post_question ?>';
</script>
<script type="text/javascript" 
        src="<?= $config->full_root_path ?>/posts/media/global_functions~v<?= $config->scripts_version ?>.js"></script>
