<?php
/**
 * Post editing form
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var \SimpleXMLElement $language
 * @var module[] $modules
 */

use hng2_base\config;
use hng2_base\module;
use hng2_modules\categories\categories_repository;
use hng2_modules\posts\posts_repository;
use hng2_modules\posts\toolbox;

$posts_repository      = new posts_repository();
$posts_toolbox         = new toolbox();
$categories_repository = new categories_repository();
$default_category      = $categories_repository->get("0000000000000");

$tree_filter = array(
    "id_category <> '0000000000000'",
    "( visibility = 'public' or visibility = 'users' or (visibility  = 'level_based' and '{$account->level}' >= min_level) )"
);
$categories_count = $categories_repository->get_record_count($tree_filter);
?>

<div id="post_form_target" style="display: none;"></div>

<?
$raw_list = $settings->get("modules:posts.main_category_selector_exceptions");
if( ! empty($raw_list) )
{
    $exception_slugs = array();
    foreach(explode("\n", trim($raw_list)) as $line)
    {
        $line = trim($line);
        if( empty($line) ) continue;
        
        list($slug, $level) = preg_split('/\s+-\s+/', $line);
        if( $account->level >= $level ) continue;
        
        $exception_slugs[] = "'$slug'";
    }
    
    if( ! empty($exception_slugs) )
    {
        $res = $categories_repository->find(
            array("slug in (" . implode(", ", $exception_slugs) . ")"), 0, 0, ""
        );
        
        if( ! empty($res) )
        {
            echo "\n<div id='category_exceptions' style='display: none;'>\n";
            foreach($res as $category_record)
                echo "    <div data-id-category='{$category_record->id_category}'>{$category_record->slug}</div>\n";
            echo "</div>";
        }
    }
}

$custom_fields_editing_level = $settings->get("modules:posts.level_allowed_to_edit_custom_fields");
if( empty($custom_fields_editing_level) )
{
    $custom_fields_editing_level = config::MODERATOR_USER_LEVEL;
    $settings->set("modules:posts.level_allowed_to_edit_custom_fields", $custom_fields_editing_level);
}
?>

<form name="post_form" id="post_form" method="post" data-preview-mode=""
      action="<?= $config->full_root_path ?>/posts/scripts/save.php">
    
    <input type="hidden" name="id_post" value="">
    <input type="hidden" name="status" value="draft">
    <input type="hidden" name="is_autosave" value="false">
    <input type="hidden" name="is_preview" value="false">
    
    <!-- BCM additions -->
    <input type="hidden" name="bcm_platform"      value="<?= $_REQUEST["bcm_platform"] ?>">
    <input type="hidden" name="bcm_version"       value="<?= $_REQUEST["bcm_version"] ?>">
    <input type="hidden" name="bcm_access_token"  value="<?= $_REQUEST["bcm_access_token"] ?>">
    <input type="hidden" name="bcm_force_session" value="<?= $_REQUEST["bcm_force_session"] ?>">
    <input type="hidden" name="bcm_output_type"   value="<?= $_REQUEST["bcm_output_type"] ?>">
    
    <div class="field" data-field="title">
        <textarea name="title" class="expandible_textarea no_fw_font" 
                  placeholder="<?= $current_module->language->form->fields->title->placeholder ?>"></textarea>
    </div>
    
    <? $style = $categories_count == 0 ? "display: none" : ""; ?>
    <div class="field" id="main_category_selector_container" data-field="category" style="<?= $style ?>">
        <select name="main_category" class="big_monospace_select"  onchange="toggle_post_category_message(this)">
            <option selected value="<?= $default_category->id_category ?>">
                <?= $default_category->title ?>: <?= $default_category->description ?>
            </option>
        </select>
        
        <div class="category_messages">
            <? foreach( $posts_toolbox->get_per_category_messages() as $category_id => $text ): ?>
                <div class="message framed_content state_highlight" style="display: none;"
                     data-category-id="<?= $category_id ?>">
                    <span class="fa fa-info-circle"></span>
                    <?= $text ?>
                </div>
            <? endforeach; ?>
        </div>
    </div>
    
    <? $modules["posts"]->load_extensions("editor_form", "below_main_category_selector"); ?>
    
    <div class="field tinymce_container" data-field="content">
        <textarea name="content" id="post_content_editor" class="tinymce_full"
                  placeholder="<?= $current_module->language->form->fields->content->placeholder ?>"></textarea>
    </div>
    
    <? $modules["posts"]->load_extensions("editor_form", "below_editor"); ?>
    
    <div class="field framed_content" data-field="controls" style="display: none;">
        <div class="multicol cols-3 biggest-col">
            
            <div class="col subfield">
                
                <div class="post_excerpt">
                    <div class="caption"><?= $current_module->language->form->fields->excerpt->caption ?></div>
                    <div class="input">
                        <textarea name="excerpt" class="expandible_textarea no_fw_font" style="min-height: 150px;"
                                  placeholder="<?= $current_module->language->form->fields->excerpt->placeholder ?>"></textarea>
                    </div>
                </div>
                
                <div class="parent_post_area" style="display: none">
                    <input type="hidden" name="parent_post" value="">
                    <div class="caption"><?= $current_module->language->form->fields->parent_post->caption ?></div>
                    <div class="input">
                        <div class="parent_post_title framed_content" style="margin: 0;"></div>
                    </div>
                </div>
                
                <? $modules["posts"]->load_extensions("editor_form", "options_left_column"); ?>
                
            </div>
            
            <div class="col subfield">
                
                <div class="post_visibility">
                    <div class="caption"><?= $current_module->language->form->fields->visibility->caption ?></div>
                    <div class="input">
                        <select name="visibility">
                            <? foreach($current_module->language->form->fields->visibility->options->option as $option): ?>
                                <option value="<?= $option["value"] ?>" data-caption="<?= trim($option) ?>">
                                    <?= trim($option) ?>
                                </option>
                            <? endforeach; ?>
                        </select>
                    </div>
                </div>
                
                <div class="post_password">
                    <? $info_handler = "post_composer_password_info"; ?>
                    <div class="caption">
                        <?= $current_module->language->form->fields->password->caption ?>
                        <i class="pseudo_link" onclick="toggle_info_section('<?= $info_handler ?>')"><i
                           class="fa fa-info-circle" title="<?= $language->toggle_info ?>"></i></i>
                    </div>
                    <div class="input info_handler" id="<?= $info_handler ?>"
                         style="<? if($account->engine_prefs[$info_handler] == "hidden") echo "display: none;"; ?>">
                        <i class="fa fa-info-circle"></i>
                        <?= $current_module->language->form->fields->password->info ?>
                    </div>
                    <div class="input">
                        <input type="hidden" name="password" placeholder="<?= $current_module->language->form->fields->password->placeholder ?>">
                        <div class="password_control">
                            <span class="current_password framed_content inlined" data-none-caption="<?= $current_module->language->form->fields->password->none ?>"></span>
                            &nbsp;<span class="set pseudo_link" onclick="edit_post_password()"><i class="fa fa-pencil fa-fw"></i></span>
                            &nbsp;<span class="remove pseudo_link" onclick="set_post_password('')"><i class="fa fa-times fa-fw"></i></span>
                        </div>
                    </div>
                </div>
                
                <div class="post_slug">
                    <div class="field" data-field="slug">
                        <div class="caption">
                            <?= $current_module->language->form->fields->slug->caption ?>
                        </div>
                        <div class="input">
                            <input type="text" name="slug" value="" maxlength="128"
                                   placeholder="<?= $current_module->language->form->fields->slug->placeholder ?>">
                        </div>
                    </div>
                </div>
                
                <? $modules["posts"]->load_extensions("editor_form", "options_middle_column"); ?>
                
                <? if( $account->level >= config::MODERATOR_USER_LEVEL ): ?>
                    <div class="post_allow_comments">
                        <div class="caption important">
                            <?= $current_module->language->form->fields->editor_options ?>
                        </div>
                        <div class="input">
                            <span class="pull-right nowrap">
                                <?= $language->no ?>
                                <span class='fa-pseudo-switch' data-input-name='allow_comments' data-value-on='1' data-value-off='0' onclick='toggle_fa_pseudo_switch(this);'>
                                    <input type='hidden' name='allow_comments' value='1'>
                                    <span class='toggler toggle-on  fa fa-toggle-on'  style='display: none'></span>
                                    <span class='toggler toggle-off fa fa-toggle-off' style=''></span>
                                </span>
                                <?= $language->yes ?>
                            </span>
                            <span class="greengo">
                                <?= $current_module->language->form->fields->allow_comments->caption ?>
                            </span>
                        </div>
                    </div>
                    
                    <div class="post_pin_to_home">
                        <div class="input">
                            <span class="pull-right nowrap">
                                <?= $language->no ?>
                                <span class='fa-pseudo-switch' data-input-name='pin_to_home' data-value-on='1' data-value-off='0' onclick='toggle_fa_pseudo_switch(this);'>
                                    <input type='hidden' name='pin_to_home' value='0'>
                                    <span class='toggler toggle-on  fa fa-toggle-on'  style=''></span>
                                    <span class='toggler toggle-off fa fa-toggle-off' style='display: none'></span>
                                </span>
                                <?= $language->yes ?>
                            </span>
                            <span class="greengo">
                                <?= $current_module->language->form->fields->pin_to_home->caption ?>
                            </span>
                        </div>
                    </div>
                    
                    <div class="post_pin_to_main_category_index">
                        <div class="input">
                            <span class="pull-right nowrap">
                                <?= $language->no ?>
                                <span class='fa-pseudo-switch' data-input-name='pin_to_main_category_index' data-value-on='1' data-value-off='0' onclick='toggle_fa_pseudo_switch(this);'>
                                    <input type='hidden' name='pin_to_main_category_index' value='0'>
                                    <span class='toggler toggle-on  fa fa-toggle-on'  style=''></span>
                                    <span class='toggler toggle-off fa fa-toggle-off' style='display: none'></span>
                                </span>
                                <?= $language->yes ?>
                            </span>
                            <span class="greengo">
                                <?= $current_module->language->form->fields->pin_to_main_category_index->caption ?>
                            </span>
                        </div>
                    </div>
                <? endif; ?>
                
            </div>
            
            <div class="col subfield featured_image">
                
                <div class="post_featured_image">
                    <input type="hidden" name="id_featured_image" value="">
                    <div class="caption">
                        <?= $current_module->language->form->fields->featured_image->caption ?>
                    </div>
                    <div class="input">
                        <div class="thumbnail preview featured_image pseudo_link" 
                             onclick="set_post_featured_image()">
                            <img src="<?= $config->full_root_path ?>/media/missing_image.png"
                                 data-empty-src="<?= $config->full_root_path ?>/media/missing_image.png">
                        </div>
                        <div align="center">
                            <span class="pseudo_link" onclick="set_post_featured_image()">
                                <?= $current_module->language->form->fields->featured_image->set ?>
                            </span>
                            &nbsp;
                            <span class="pseudo_link" onclick="remove_post_featured_image()">
                                <?= $current_module->language->form->fields->featured_image->remove ?>
                            </span>
                        </div>
                    </div>
                </div>
                
                <div class="post_publishing_date">
                    <div class="field" data-field="publishing_date">
                        <input type="hidden" name="publishing_date" value="">
                        <div class="caption">
                            <?= $current_module->language->form->fields->publishing_date->caption ?>
                        </div>
                        <div class="input">
                            <span class="values">
                                <span class="framed_content inlined automatic">
                                    <?= $current_module->language->form->fields->publishing_date->automatic ?>
                                </span>
                                <span class="framed_content inlined specific" style="display: none">&mdash;</span>
                            </span>
                            
                            <span class="controls">
                                <span class="trigger pseudo_link" onclick="show_scheduling_controls()"><i class="fa fa-pencil"></i>
                                    <?= $current_module->language->form->fields->publishing_date->schedule ?></span>
                                
                                <span class="fieldset framed_content state_active" style="display: none">
                                    <span class="inline-block nowrap" style="margin: 0 5px 5px 0">
                                        <input type="text" readonly name="schedule[date]" id="schedule_date" value=""
                                               maxlength="10" style="width: 100px; text-align: center;">
                                    </span>
                                    
                                    <span class="inline-block nowrap" style="margin: 0 5px 5px 0">
                                        <input type="text" name="schedule[time]" value="" maxlength="5"
                                               style="width: 60px; text-align: center;">
                                    </span>
                                    
                                    <span class="inline-block nowrap">
                                        <button onclick="set_schedule_date(); return false;"><i class="fa fa-check"></i></button>
                                        <button onclick="cancel_schedule_edition(); return false;"><i class="fa fa-times"></i></button>
                                        <button onclick="remove_automatic_schedule(); return false;">
                                            <?= $current_module->language->form->fields->publishing_date->immediate ?>
                                        </button>
                                    </span><br>
                                </span>
                            </span>
                        </div>
                    </div>
                </div>
                
                <? $modules["posts"]->load_extensions("editor_form", "options_right_column"); ?>
                
            </div>
            
        </div>
        
        <? if( $account->level >= $custom_fields_editing_level ): ?>
            <fieldset>
                <legend><?= $current_module->language->form->custom_fields->title ?></legend>
                <div id="custom_fields_target" class="multicol cols-2"></div>
                <button onclick="add_custom_field(); return false;">
                    <i class="fa fa-plus"></i>
                    <?= $current_module->language->form->custom_fields->add ?>
                </button>
            </fieldset>
        <? endif; ?>
    </div>
    
    <div class="post_buttons alignright clearfix">
        <span class="post_autosave_status pull-left">
            <span class="saving" style="display: none">
                <span class="fa fa-spinner fa-pulse"></span>
                <?= $current_module->language->saving ?>
            </span>
            <span class="saved" style="display: none">
                <span class="fa fa-check"></span>
                <?= $current_module->language->saved ?>
            </span>
            <span class="error" style="display: none;">
                <span class="fa fa-warning"></span>
                <span class="message"></span>
            </span>
        </span>
        
        <button onclick="$('#post_form').find('.field[data-field=controls]').toggle(); return false;"
                title="<?= $current_module->language->form->fields->controls_toggle->title ?>">
            <span class="fa fa-check-square-o"></span>
            <?= $current_module->language->form->fields->controls_toggle->caption ?>
        </button>
        
        <button type="reset" onclick="hide_post_form()">
            <span class="fa fa-ban"></span>
            <?= $language->cancel ?>
        </button>
        
        <? if( $_GET["hide_post_preview"] != "true" ): ?>
            <button type="submit" data-save-type="save_draft" onclick="prepare_post_preview()">
                <span class="fa fa-search"></span>
                <?= $current_module->language->form->buttons->preview ?>
            </button>
        <? endif; ?>
        <button type="submit" data-save-type="save_draft"
                onclick="$('#post_form').find('input[name=status]').val('draft')">
            <span class="fa fa-save"></span>
            <?= $current_module->language->form->buttons->save_draft ?>
        </button>
        <button type="submit" data-save-type="save">
            <span class="fa fa-save"></span>
            <?= $language->save ?>
        </button>
        <button type="submit" data-save-type="publish"
                onclick="$('#post_form').find('input[name=status]').val('published')">
            <?= $current_module->language->form->buttons->publish ?>
            <span class="fa fa-play"></span>
        </button>
    </div>
</form>

<? if( $account->level >= $custom_fields_editing_level ): ?>
    <div id="custom_field_template" style="display: none;">
        <div class="col custom_field">
            <div class="caption nowrap">
                <input type="text" name="custom_field_names[]" value=""
                       placeholder="<?= $current_module->language->form->custom_fields->name_placeholder ?>"
                       style="width: calc(100% - 16px)">
                <i class="fa fa-trash pseudo_link" onclick="$(this).closest('.custom_field').remove()"></i>
            </div>
            <div class="input">
                <textarea name="custom_field_values[]" rows="3" wrap="off"
                          placeholder="<?= $current_module->language->form->custom_fields->value_placeholder ?>"></textarea>
            </div>
        </div>
    </div>
<? endif; ?>

<? $modules["posts"]->load_extensions("editor_form", "outside"); ?>
                
