<?php
/**
 * @file ting_object.tpl.php
 *
 * Template to render a collection objects from the Ting database.
 */
?>
<div class="ting-overview clearfix">
  <h1><?php print $ting_title ?></h1>
  <?php if (sizeof($ting_creators_links) == 1) { ?>
  <div class='creator'>
    <span class='byline'><?php echo ucfirst(t('by')); ?></span>
    <?php print implode(', ', $ting_creators_links); ?>
    <span class='date'>(<?php print $ting_publication_date; ?>)</span>
  </div>
  <?php } ?>

  <p class="abstract"><?php print $ting_abstract; ?></p>

  <div class="ting-details clearfix">
    <?php print $ting_details; ?>
 </div>

  <?php  if (count($sorted_collection) > 1) { ?>
    <div class='material-links'>
      <span class='title'><?php echo t('Go to material:'); ?></span>
      <?php
      foreach ($sorted_collection as $type => $objects) {
        $material_links[] = '<span class="link"><a href="#'.$type.'">'.t($type).'</a></span>';
      }
      print implode(", ", $material_links);
      ?>
    </div>
  <?php } ?>
</div>


<?php print $ting_list; ?>
