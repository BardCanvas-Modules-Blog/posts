
<!-- posts: messages for gallery embeds -->

<div id="post_gallery_embed_strings" style="display: none;">
    <div class="invalid_type_for_image"><?= $this_module->language->messages->gallery_embeds->invalid_type_for_image ?></div>
    <div class="invalid_type_for_video"><?= $this_module->language->messages->gallery_embeds->invalid_type_for_video ?></div>
</div>

<?
if( ! is_mobile() ) return;
if( ! isset($account) ) return;
if( ! $account->_exists ) return;
if( basename(__FILE__) == "quick_single.php" ) return;
if( $template->layout == "popup" ) return;
if( $template->layout == "admin" ) return;
if( $settings->get("modules:posts.disable_quick_post_button") == "true" ) return;
?>

<!-- Quick single button -->

<a id="quick_post_floating_trigger"
   class="fa fa-bolt fa-fw"  href="<?= $this_module->get_url() ?>/quick_single.php"
   style="text-decoration: none;"></a>
