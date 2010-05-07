<?php
//determine first actual search - i.e. it has a title
//system form can contain empty elements
$firstIndex = 0;
foreach ($searches as $i => $search)
{
	if ($search['title'])
	{
		$firstIndex = $i;
		break;
	}
}
?>
<div class="ting-search-carousel">
	<ul class="search-results">
		<?php foreach ($searches as $i => $search) :?>
		  <?php if ($search['title']) : ?>
				<li class="<?php echo ($i == $firstIndex) ? 'active' : ''; ?>">
					<div class="subtitle">
						<?php echo $search['subtitle']; ?>
					</div>							
					<ul class="jcarousel-skin-ting-search-carousel">
					</ul>
				</li>
			<?php endif; ?>
		<?php endforeach; ?>
	</ul>
	
	<ul class="search-controller">
		<?php foreach ($searches as $i => $search) : ?>
		  <?php if ($search['title']) :?>
				<li class="<?php echo ($i == $firstIndex) ? 'active' : ''; ?>">
					<a href="#"><?php echo $search['title'] ?></a>
				</li>
			<?php endif; ?>
		<?php endforeach; ?>
	</ul>
</div>
