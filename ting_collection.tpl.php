<?php
// $Id$
/**
 * @file ting_object.tpl.php
 *
 * Template to render a collection objects from the Ting database.
 *
 * Available variables:
 * - $collection: The TingClientObjectCollection instance we're rendering.
 * - $sorted_collection: Array of TingClientObject instances sorted by type.
 * - $ting_list: Rendered ting objects.
 */
?>
<div class="ting-overview clearfix">
  <h1><?php print check_plain($collection->title); ?></h1>
  <?php if ($collection->creators) { ?>
  <div class='creator'>
    <span class='byline'><?php echo ucfirst(t('by')); ?></span>
    <?php
    $creators = array();
    foreach ($collection->creators as $i => $creator) {
      $creators[] = l($creator, 'search/ting/' . $creator, array('attributes' => array('class' => 'author')));
    }
    print implode(', ', $creators);
    ?>
    <span class='date'>(<?php print $collection->date; ?>)</span>
  </div>
  <?php } ?>

  <p><?php print check_plain($collection->abstract); ?></p>

  <div class='terms'>
    <span class='title'><?php echo t('Terms:'); ?></span>
    <?php
    $subjects = array();
    foreach ($collection->subjects as $subject) {
      $subjects[] = "<span class='term'>". l($subject, 'search/ting/'. $subject) ."</span>";
    }
    print implode(', ', $subjects);
    ?>
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
