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

<link rel="stylesheet" href="<?= $this_module->get_url() ?>/media/global_styles~v1.0.0-1.css">
<script type="text/javascript">
    var abandon_post_question = '<?= $this_module->language->messages->abandon_post_question ?>';
    var toggle_post_category_message_callbacks;
</script>
<script type="text/javascript" src="<?= $this_module->get_url() ?>/media/global_functions~v1.2.0-1.js"></script>
