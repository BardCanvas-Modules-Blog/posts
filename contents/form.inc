<?php
/**
 * Post editing form
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *             
 * @var module $current_module
 */

use hng2_base\module;
use hng2_modules\categories\categories_repository;

$categories_repository = new categories_repository();
$default_category      = $categories_repository->get("0000000000000");

$tree_filter = array(
    "id_category <> '0000000000000'",
    "( visibility = 'public' or visibility = 'users' or (visibility  = 'level_based' and '{$account->level}' >= min_level) )"
);

$categories_tree = $categories_repository->get_as_tree_for_select($tree_filter, "title asc", true);
?>

<div id="post_form_target" style="display: none;"></div>

<form name="post_form" id="post_form" method="post"
      action="<?= $config->full_root_path ?>/posts/scripts/save.php">
    
    <input type="hidden" name="id_post" value="">
    
    <div class="field" data-field="title">
        <textarea name="title" class="expandible_textarea no_fw_font" 
                  placeholder="<?= $current_module->language->form->fields->title->placeholder ?>"></textarea>
    </div>
    
    <? $style = count($categories_tree) == 0 ? "display: none" : ""; ?>
    <div class="field" data-field="category" style="<?= $style ?>">
        <select name="main_category" class="fixed_font">
            <option selected value="<?= $default_category->id_category ?>">
                <?= $default_category->title ?>: <?= $default_category->description ?>
            </option>
            <? foreach($categories_tree as $key => $val): ?>
                <option value="<?= $key ?>"><?= $val ?></option>
            <? endforeach; ?>
        </select>
    </div>
    
    <div class="field" data-field="content">
        <textarea name="content" id="post_content_editor" class="tinymce"></textarea>
    </div>
    
    <div class="field" data-field="excerpt" style="display: none;">
        <textarea name="excerpt" class="expandible_textarea no_fw_font"
                  placeholder="<?= $current_module->language->form->fields->excerpt->placeholder ?>"></textarea>
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
            <button type="submit">
                <span class="fa fa-check"></span>
                <?= $language->save ?>
            </button>
        </span>
    </div>
    
</form>
