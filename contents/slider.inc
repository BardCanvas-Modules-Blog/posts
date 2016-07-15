<?
/**
 * Home slider
 *
 * @package    BardCanvas
 * @subpackage posts
 * @author     Alejandro Caballero - lava.caballero@gmail.com
 *             
 * @var posts_repository $posts_repository
 */

use hng2_modules\posts\posts_repository;

if( $template->layout != "home" ) return;
?>

<div id="posts_slider">
    
    <article>
        This is the posts slider and should be shown only at the home.
    </article>
    
</div>
