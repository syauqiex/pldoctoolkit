package org.spbu.pldoctoolkit.drlvisual.diagram.edit.parts;

import org.eclipse.gmf.runtime.diagram.ui.editpolicies.EditPolicyRoles;
import org.eclipse.gmf.runtime.notation.View;
import org.eclipse.draw2d.Connection;

import org.eclipse.gef.EditPolicy;

import org.eclipse.gmf.runtime.diagram.ui.editparts.ConnectionNodeEditPart;

import org.eclipse.gmf.runtime.draw2d.ui.figures.PolylineConnectionEx;

import org.spbu.pldoctoolkit.drlvisual.diagram.edit.policies.DRLModelReferenceConnectionEditPolicy;
import org.spbu.pldoctoolkit.drlvisual.diagram.edit.policies.SubelementedElementElementsItemSemanticEditPolicy;

/**
 * @generated
 */
public class SubelementedElementElementsEditPart extends ConnectionNodeEditPart {

	/**
	 * @generated
	 */
	public static final int VISUAL_ID = 3003;

	/**
	 * @generated
	 */
	public SubelementedElementElementsEditPart(View view) {
		super(view);
	}

	/**
	 * @generated
	 */
	protected void createDefaultEditPolicies() {
		super.createDefaultEditPolicies();
		installEditPolicy(EditPolicy.CONNECTION_ROLE,
				new DRLModelReferenceConnectionEditPolicy());
		installEditPolicy(EditPolicyRoles.SEMANTIC_ROLE,
				new SubelementedElementElementsItemSemanticEditPolicy());
	}

	/**
	 * Creates figure for this edit part.
	 * 
	 * Body of this method does not depend on settings in generation model
	 * so you may safely remove <i>generated</i> tag and modify it.
	 * 
	 * @generated
	 */
	protected Connection createConnectionFigure() {
		return new PolylineConnectionEx();
	}

}