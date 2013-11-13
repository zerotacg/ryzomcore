{extends file="layout.tpl"}
{block name=content}

<div class="row-fluid">
	<div class="span12 center login-header">
		<a href="?"><img src="img/mainlogo.png"/></a> 
	</div><!--/span-->
</div><!--/row-->

<div class="row-fluid">
	<div class="well span5 center login-box">
		<div class="alert alert-info">
			{$login_info}
		</div>
		<form method="post" action="index.php{if isset($getstring)}{$getstring}{/if}" class="form-horizontal">
			<fieldset>
				<div data-rel="tooltip" class="input-prepend" data-original-title="Username">
					<span class="add-on"><i class="icon-user"></i></span><input type="text" value="" id="Username" name="Username" class="input-large span10" placeholder="Username">
				</div>
				<div class="clearfix"></div>

				<div data-rel="tooltip" class="input-prepend" data-original-title="Password">
					<span class="add-on"><i class="icon-lock"></i></span><input type="password" value="" id="Password" name="Password" class="input-large span10" placeholder="Password">
				</div>
				<div class="clearfix"></div>
		
				<p class="center span5">
				<input type="hidden" name="function" value="do_install">
				<button class="btn btn-primary" type="submit">Run Install</button>
				</p>
			</fieldset>
		</form>
		
		{if isset($login_error) and $login_error eq "TRUE"}
		<div class="alert alert-error">
			<button type="button" class="close" data-dismiss="alert">×</button>
			<strong>{$login_error_message}</strong>
		</div>
		{/if}
	</div><!--/span-->
</div>
{/block}
