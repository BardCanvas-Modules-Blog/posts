<?php
/**
 * Post editing form
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 */

use hng2_modules\categories\categories_repository;

$categories_repository = new categories_repository();
$default_category      = $categories_repository->get("0000000000000");

$tree_filter = array(
    "id_category <> '0000000000000'",
    "( visibility = 'public' or visibility = 'users' or (visibility  = 'level_based' and '{$account->level}' >= min_level) )"
);
$categories_count = $categories_repository->get_record_count($tree_filter);
?>

<div id="post_form_target" style="display: none;"></div>

<form name="post_form" id="post_form" method="post"
      action="<?= $config->full_root_path ?>/posts/scripts/save.php">
    
    <input type="hidden" name="id_post" value="">
    <input type="hidden" name="status" value="draft">
    
    <div class="field" data-field="title">
        <textarea name="title" class="expandible_textarea no_fw_font" 
                  placeholder="<?= $current_module->language->form->fields->title->placeholder ?>"></textarea>
    </div>
    
    <? $style = $categories_count == 0 ? "display: none" : ""; ?>
    <div class="field" id="main_category_selector_container" data-field="category" style="<?= $style ?>">
        <select name="main_category" class="fixed_font">
            <option selected value="<?= $default_category->id_category ?>">
                <?= $default_category->title ?>: <?= $default_category->description ?>
            </option>
        </select>
    </div>
    
    <div class="field" data-field="content">
        <textarea name="content" id="post_content_editor" class="tinymce"
                  placeholder="<?= $current_module->language->form->fields->content->placeholder ?>"></textarea>
    </div>
    
    <div class="field" data-field="excerpt" style="display: none;">
        <div class="caption"><?= $current_module->language->form->fields->excerpt->caption ?></div>
        <div class="input">
            <textarea name="excerpt" class="expandible_textarea no_fw_font"
                      placeholder="<?= $current_module->language->form->fields->excerpt->placeholder ?>"></textarea>
        </div>
    </div>
    
    <div class="field post_addons_bar clearfix">
        <? foreach($current_module->language->form->post_addons->addon as $addon): ?>
            <span class="post_addon pseudo_link" title="<?= $addon->title ?>"
                  data-function="<?= $addon["function"] ?>" data-related-field="<?= $addon["data_related_field"] ?>"
                  onclick="trigger_post_form_addon(this)"><span 
                    class="fa <?= $addon["icon"] ?> fa-fw"></span><span 
                    class="caption"><?= $addon->caption ?></span></span>
        <? endforeach; ?>
        
        <span class="post_buttons pull-right">
            <button type="reset" onclick="hide_post_form()">
                <span class="fa fa-ban"></span>
                <?= $language->cancel ?>
            </button>
            
            <button type="submit" data-save-type="save_draft"
                    onclick="$('#post_form').find('input[name=status]').val('draft')">
                <span class="fa fa-check"></span>
                <?= $current_module->language->form->buttons->save_draft ?>
            </button>
            <button type="submit" data-save-type="save">
                <span class="fa fa-check"></span>
                <?= $language->save ?>
            </button>
            <button type="submit" data-save-type="publish"
                    onclick="$('#post_form').find('input[name=status]').val('published')">
                <span class="fa fa-check"></span>
                <?= $current_module->language->form->buttons->publish ?>
            </button>
        </span>
    </div>
    
</form>
