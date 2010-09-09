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
 */
?>
<!--ting-collection-->
<div id="ting-collection">
	<div class="content-left">

		<div class="tab-navigation">
			<ul>
				<li class="active"><a href="#">Materialer</a></li>
			</ul>
		</div>

		<div class="tab-navigation-main">
			<div class="tab-navigation-main-inner">
				<div class="ting-overview clearfix">
          <h1><?php print check_plain($collection->title); ?></h1>
	
          <?php if ($collection->creators) { ?>
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
          <?php } ?>

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


				<?php
 				foreach ($sorted_collection as $type => $objects) {		
          if(count($sorted_collection) > 1){ 
					  print '<h2>'.$type.'<a name="'.$type.'"></a></h2>';
 					}

					foreach ($objects as $tingClientObject) {
				    // now display all the materials
				?>

				<div id="ting-item-<?php print $tingClientObject->localId; ?>" class="ting-item clearfix">

          <div class="content clearfix">
  		  		<div class="picture">
  						<?php $image_url = ting_covers_object_url($tingClientObject, '80_x'); ?>
  						<?php if ($image_url) { ?>
  							<?php print theme('image', $image_url, '', '', null, false); ?>
  						<?php } ?>
  					</div>

  				  <div class="info">
				  		<span class='date'><?php print $tingClientObject->record['dc:date'][''][0]; ?></span> 
  						<h3><?php print l($tingClientObject->title, $tingClientObject->url, array('attributes' => array('class' => 'alternative'))); ?></h3>

  						<em><?php echo t('by'); ?></em>
  						<?php echo l($tingClientObject->creators[0], 'search/ting/'. $tingClientObject->creators[0], array('attributes' => array('class' => 'author alternative'))); ?>
	
  						<div class='language'><?php echo t('Language') . ': ' . $tingClientObject->language; ?></div>
  						<?php
  						for ($i = 1; $i < count($tingClientObject->creators); $i++) {
  							if($extradesc = $tingClientObject->creators[$i]) { print "<p>".$extradesc."</p>"; }
  						}
  						?>

  						<div class="more">
                <?php print l(t('More information'), $tingClientObject->url, array('attributes' => array('class' => 'more-link')) ); ?>
  						</div>
              <?php if ($tingClientObject->type != 'Netdokument') { ?>
                <div class="ting-status waiting">Afventer data…</div>
              <?php } ?>
  					</div>

          </div>
          
          <?php if ($buttons[$tingClientObject->id]) :?>
						<div class="ting-object-buttons">
	            <?php print theme('item_list', $buttons[$tingClientObject->id], NULL, 'ul', array('class' => 'buttons')) ?>
						</div>
					<?php endif; ?>

				</div>

				<?php 
					} // foreach objects
				} //foreach collection
				?>

        <?php
        $referenced_nodes = ting_reference_nodes($collection);
        if ($referenced_nodes) {
          print '<h3>Omtale på websitet</h3>';
          foreach ($referenced_nodes as $node) {
            print node_view($node, TRUE);
          }
        }
        ?>
			</div>	
		</div>
	</div>

</div>

<!--/ting-collection-->