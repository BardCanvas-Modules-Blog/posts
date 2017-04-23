<?php
/**
 * Accounts extender: adds post stats on the user profile 
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 * 
 * @var account $author
 */

use hng2_base\account;
use hng2_modules\posts\posts_repository;

global $template, $settings, $account, $modules;

$author = $template->get("user_profile_account");
$repository = new posts_repository();

#
# Post counts by category
#

$rows = $repository->get_category_counts(0, 0, "title asc", $author->id_account);
# echo "<pre>query := {$repository->get_last_query()}</pre>";
# echo "<pre>" . print_r($rows, true) . "</pre>";

if( ! empty($rows) ):
    $wasuuup = md5(mt_rand(1, 65535)); ?>
    
    <section>
        <h2>
            <?= $this_module->language->user_profile_sections->posts_per_category ?>
            <? if( $account->level >= $config::MODERATOR_USER_LEVEL )
                echo "<a class='prettyPhoto autosize_chart'
                         href='{$this_module->get_url()}/posts_per_day_chart.php?id_author={$author->id_account}&width=620&height=380&wasuuup=$wasuuup'
                         ><i class='fa fa-bar-chart fa-fw'></i></a>"; ?>
        </h2>
        <div class="framed_content user_posts_by_category">
            <ul>
                <? foreach($rows as $row): if($row->count == 0) continue; ?>
                    <li>
                        <?= $row->title ?>: <?= $row->count ?>
                        <? if( $modules["search"]->enabled )
                            echo "<a href='{$config->full_root_path}/?s=%25&cat={$row->id_category}&user={$author->user_name}' target='_blank'><i class='fa fa-search fa-fw'></i></a>"; ?>
                        <? if( $account->level >= $config::MODERATOR_USER_LEVEL )
                            echo "<a class='prettyPhoto autosize_chart'
                                     href='{$this_module->get_url()}/posts_per_day_chart.php?id_author={$author->id_account}&id_category={$row->id_category}&width=620&height=380&wasuuup=$wasuuup'
                                     ><i class='fa fa-bar-chart fa-fw'></i></a>"; ?>
                    </li>
                <? endforeach; ?>
            </ul>
        </div>
    </section>
    
    <script type="text/javascript">
        $(document).ready(function() {
            $('.autosize_chart').each(function()
            {
                var width  = Math.round($(window).width()  - 100);
                var height = Math.round($(window).height() - 150);
                var href   = $(this).attr('href');
                
                href = href.replace(/width=[0-9]+/,  'width='  + width);
                href = href.replace(/height=[0-9]+/, 'height=' + height);
                
                $(this).attr('href', href);
            });
        });
    </script>
    
    <? if( $account->level >= $config::MODERATOR_USER_LEVEL ): ?>
        <section>
            <h2>
                <?= replace_escaped_objects($this_module->language->user_profile_sections->charts->all, array('{$user}' => convert_emojis($author->display_name))) ?>
            </h2>
            <div class="framed_content user_posts_chart">
                <a class="prettyPhoto autosize_chart"
                   href="<?= $this_module->get_url() ?>/posts_per_day_chart.php?id_author=<?= $author->id_account ?>&width=620&height=200&wasuuup=<?= $wasuuup ?>"><img
                   id="all_user_posts_chart" src="" width="100%" height="200"></a>
            </div>
        </section>
        
        <script type="text/javascript">
            function adjust_all_user_posts_chart()
            {
                var $img = $('#all_user_posts_chart');
                var src  = $img.closest('a').attr('href');
                
                var width  = $img.closest('.framed_content').width();
                var height = $img.attr('height');
    
                src = src.replace(/width=[0-9]+/,  'width='  + width);
                src = src.replace(/height=[0-9]+/, 'height=' + height);
                $img.attr('src', src);
            }
            
            adjust_all_user_posts_chart();
            $(window).resize(function() { adjust_all_user_posts_chart(); });
        </script>
    <? endif; ?>
    
    <?
endif;
?>
