<?php

/**
 * @file
 * Template to render a preview of Ting references.
 */
?>
<div class="ting_reference">
  <div class="picture"></div>
  <div class="info">
		<h3 class="title">
      <?php print l($collection->title, $collection->url); ?>
		</h3>
		<div class="meta">
			<?php if ($collection->creators_string) : ?>
				<span class="creator">
					<?php print t('By %creator_name', array('%creator_name' => $collection->creators_string)); ?>
				</span>
			<?php endif; ?>
			<?php if ($collection->date) : ?>
				<span class="publication_date">(<?php echo $collection->date; ?>)</span>
			<?php endif; ?>
		</div>
		<div class="types">
			<h4><?php echo t('Material types:') ?></h4>
			<ul>
			<?php foreach ($collection->types as $type): ?>
				<li class="available"><?php echo $type; ?></li>
      <?php endforeach; ?>
			</ul>
		</div>
		<?php if ($collection->abstract) : ?>
		<div class="abstract">
      <?php print check_plain($collection->abstract); ?>
		</div>
		<?php endif; ?>
		<?php if ($collection->subjects) : ?>
			<div class="subjects">
				<h4><?php echo t('Subjects:') ?></h4>
				<ul>
					<?php foreach ($collection->subjects as $subject) : ?>
						<li><?php echo $subject ?></li>
					<?php endforeach; ?>
				</ul>	
			</div>
		<?php endif; ?>
	</div>
</div>

